import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { ChunkMapTile } from "../IPC/electronIPC";
import * as THREE from "three";
import { Vector3 } from "@react-three/fiber";
export default class RegularThree {
  canvas: HTMLCanvasElement;
  // scene: THREE.Scene;
  // camera: THREE.PerspectiveCamera;
  // renderer: THREE.WebGLRenderer;

  unit = 10;
  unitWidth = this.unit;
  unitHeight = this.unit;

  constructor(canvas: HTMLCanvasElement) {
    this.canvas = canvas;
  }

  displayChunk(chunk: ChunkMapTile) {
    // while (this.scene.children.length > 0) {
    //   this.scene.remove(this.scene.children[0]);
    // }

    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(
      75,
      this.canvas.clientWidth / this.canvas.clientHeight,
      0.1,
      1000
    );

    const renderer = new THREE.WebGLRenderer({ canvas: this.canvas });
    renderer.setSize(this.canvas.clientWidth, this.canvas.clientHeight, false);

    console.log(chunk);
    const rows = chunk.data.length;
    const cols = chunk.data[0].length;

    const minVal = -5000;
    const maxVal = 5000;
    // Normalize data to range [-1, 1]
    const normalizedData = chunk.data.map(
      (row) =>
        // row.map((val) => 2 * ((val - minVal) / (maxVal - minVal)) - 1)
        row.map((val) => (val - minVal) / (maxVal - minVal) - 1) // -.5 to .5
    );
    const positions = new Float32Array(rows * cols * 3);

    const indices = new Uint32Array((rows - 1) * (cols - 1) * 6);

    let idx3 = 0; // position index for the Float32Array
    // const scale = 0.1; // Adjust this value to make the mesh larger or smaller
    // const xOffset = -((cols - 1) * scale) / 2;
    // const zOffset = -((rows - 1) * scale) / 2;
    for (let i = 0; i < rows; i++) {
      for (let j = 0; j < cols; j++) {
        // x = j, z = i, y = heightData[i][j]
        positions[idx3 + 0] =
          chunk.chunkDescription.coordinate.x * this.unitWidth +
          (j * this.unitWidth) / cols; // x
        positions[idx3 + 1] = normalizedData[i][j]; // y
        positions[idx3 + 2] =
          chunk.chunkDescription.coordinate.y * this.unitHeight +
          (i * this.unitHeight) / rows; // z
        idx3 += 3;
      }
    }

    // We go through each cell (i, j) and create two triangles:
    //  - Triangle A: (i,j), (i+1,j), (i,j+1)
    //  - Triangle B: (i+1,j), (i+1,j+1), (i,j+1)
    let idxi = 0; // index array pointer
    for (let i = 0; i < rows - 1; i++) {
      for (let j = 0; j < cols - 1; j++) {
        // top-left vertex index in the grid
        const topLeft = i * cols + j;
        const bottomLeft = (i + 1) * cols + j;
        const topRight = i * cols + (j + 1);
        const bottomRight = (i + 1) * cols + (j + 1);

        // Triangle A
        indices[idxi++] = topLeft;
        indices[idxi++] = bottomLeft;
        indices[idxi++] = topRight;

        // Triangle B
        indices[idxi++] = bottomLeft;
        indices[idxi++] = bottomRight;
        indices[idxi++] = topRight;
      }
    }

    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute("position", new THREE.BufferAttribute(positions, 3));
    geometry.setIndex(new THREE.BufferAttribute(indices, 1));

    // (optional) compute normals for proper lighting
    geometry.computeVertexNormals();

    geometry.computeBoundingBox();
    const heatGradiant = new THREE.ShaderMaterial({
      uniforms: {
        color1: {
          value: new THREE.Color("red")
        },
        color2: {
          value: new THREE.Color("purple")
        },
        bboxMin: {
          value: geometry.boundingBox.min
        },
        bboxMax: {
          value: geometry.boundingBox.max
        }
      },
      vertexShader: `
    uniform vec3 bboxMin;
    uniform vec3 bboxMax;
  
    varying vec2 vUv;

    void main() {
      vUv.y = (position.y - bboxMin.y) / (bboxMax.y - bboxMin.y);
      gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);
    }
  `,
      fragmentShader: `
    uniform vec3 color1;
    uniform vec3 color2;
  
    varying vec2 vUv;
    
    void main() {
      
      gl_FragColor = vec4(mix(color1, color2, vUv.y), 1.0);
    }
  `,
    })

    const material = new THREE.MeshStandardMaterial({
      color: 0xDB7030,
      wireframe: false,
    });

    // Add some light to see the mesh better
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(5, 5, 5);
    scene.add(directionalLight);

    // Adjust camera position to better view the terrain
    // this.camera.position.set(
    //   chunk.chunkDescription.coordinate.x * this.unitWidth + 1,
    //   0 + 1,
    //   chunk.chunkDescription.coordinate.y * this.unitHeight + 1
    // );
    // this.camera.lookAt(
    //   chunk.chunkDescription.coordinate.x * this.unitWidth,
    //   0,
    //   chunk.chunkDescription.coordinate.y * this.unitHeight
    // );

    // const mesh = new THREE.Mesh(geometry, heatGradiant);
    const mesh = new THREE.Mesh(geometry, material);
    scene.add(mesh);

    // const geometry = new THREE.BoxGeometry(1, 1, 1);
    // const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
    // const cube = new THREE.Mesh(geometry, material);
    // this.scene.add(cube);

    // this.renderer.render(this.scene, this.camera);

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(
      chunk.chunkDescription.coordinate.x * this.unitWidth +
      0.5 * this.unitWidth,
      0,
      chunk.chunkDescription.coordinate.y * this.unitHeight +
      0.5 * this.unitHeight
    );
    const offset = new THREE.Vector3(
      this.unitHeight,
      this.unit,
      this.unitWidth
    );
    camera.position.copy(controls.target).add(offset);
    controls.update();

    this.addPath(chunk.chunkDescription.coordinate.x, chunk.chunkDescription.coordinate.y, rows, cols, normalizedData, scene);

    const objectGeometry = new THREE.SphereGeometry(0.01);
    const objectMaterial = new THREE.MeshBasicMaterial({ color: 0x0000ff });
    const movingObject = new THREE.Mesh(objectGeometry, objectMaterial);
    movingObject.position.set(0, 0, 0); // Place at center of the mesh
    scene.add(movingObject);

    let t = 0; // Animation progress (0 to 1)
    const r = renderer;
    const c = camera;
    const s = scene;
    function animate() {
      // t += 0.001; // Adjust speed
      // if (t > 1) t = 0; // Loop animation

      // const position = curve.getPointAt(t);
      // movingObject.position.set(position.x, position.y, position.z);

      requestAnimationFrame(animate);
      // mesh.rotation.x += 0.01;
      // mesh.rotation.y += 0.01;
      controls.update();
      r.render(s, c);
    }
    animate();
  }

  addPath(startX: number, startY: number, width: number, height: number, data: number[][], scene: THREE.Scene) {
    // const pathPoints = [
    //   new THREE.Vector3(startX * this.unitWidth, 0, startY * this.unitHeight),
    //   new THREE.Vector3((startX + 1) * this.unitWidth, 0, (startY + 1) * this.unitHeight),
    // ];
    // const curve = new THREE.CatmullRomCurve3(pathPoints);
    // const points = curve.getPoints(50);
    // const pathGeometry = new THREE.BufferGeometry().setFromPoints(points);

    const offset = 0.01
    const positions = [];
    for (let i = 0; i < width; i++) {
      positions[i] = new THREE.Vector3(startX * this.unitWidth + i * this.unitWidth / width, data[i][i] + offset, startY * this.unitHeight + i * this.unitHeight / height);
    }


    const pathGeometry = new THREE.BufferGeometry().setFromPoints(positions);
    const pathMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
    const pathLine = new THREE.Line(pathGeometry, pathMaterial);
    scene.add(pathLine);
  }
}
