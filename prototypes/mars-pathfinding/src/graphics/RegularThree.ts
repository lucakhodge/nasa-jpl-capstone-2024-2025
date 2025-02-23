import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { Chunk } from "../IPC/electronIPC";
import * as THREE from "three";
import { Vector3 } from "@react-three/fiber";
export default class RegularThree {
  canvas: HTMLCanvasElement;

  scale = 0.005


  renderer: THREE.WebGLRenderer;
  camera: THREE.PerspectiveCamera;
  scene: THREE.Scene;
  controls: OrbitControls;

  tickTime = 0;
  movingObject: THREE.Mesh;
  path: THREE.CatmullRomCurve3;
  isPlaying = false;

  constructor(canvas: HTMLCanvasElement) {
    this.canvas = canvas;
  }

  displayChunk(chunk: Chunk) {
    // // Uncomment below to clear scene (not needed in current setup)
    // while (this.scene.children.length > 0) {
    //   this.scene.remove(this.scene.children[0]);
    // }

    // create scene, camera, and renderer
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(
      75,
      this.canvas.clientWidth / this.canvas.clientHeight,
      0.1,
      1000
    );
    const renderer = new THREE.WebGLRenderer({ canvas: this.canvas });
    renderer.setSize(this.canvas.clientWidth, this.canvas.clientHeight, false);


    const rows = chunk.data.length;
    const cols = chunk.data[0].length;
    const minVal = -5000;
    const maxVal = 5000;
    // Normalize data 
    const normalizedData = chunk.data.map(
      (row) =>
        // row.map((val) => 2 * ((val - minVal) / (maxVal - minVal)) - 1)
        row.map((val) => (val - minVal) / (maxVal - minVal) - 1) // -.5 to .5
    );

    // create points for mesh
    const positions = new Float32Array(rows * cols * 3);
    const indices = new Uint32Array((rows - 1) * (cols - 1) * 6);
    let idx3 = 0; // position index for the Float32Array
    for (let i = 0; i < rows; i++) {
      for (let j = 0; j < cols; j++) {
        positions[idx3 + 0] =
          (chunk.description.coordinate.x + j) * this.scale;
        positions[idx3 + 1] = normalizedData[i][j];
        positions[idx3 + 2] =
          (chunk.description.coordinate.y + i) * this.scale;
        idx3 += 3;
      }
    }
    //create triangles for mesh
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

    // create mesh
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute("position", new THREE.BufferAttribute(positions, 3));
    geometry.setIndex(new THREE.BufferAttribute(indices, 1));
    // compute normals for proper lighting
    geometry.computeVertexNormals();

    // material for mesh
    const material = new THREE.MeshStandardMaterial({
      color: 0xDB7030,
      wireframe: false,
    });
    // Add light to make mesh look better
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    scene.add(ambientLight);
    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(5, 5, 5);
    scene.add(directionalLight);

    // const mesh = new THREE.Mesh(geometry, heatGradiant);
    const mesh = new THREE.Mesh(geometry, material);
    scene.add(mesh);

    //add controls for obiting
    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(
      (chunk.description.coordinate.x + chunk.description.size.width / 2) * this.scale,
      0,
      (chunk.description.coordinate.y + chunk.description.size.height / 2) * this.scale,
    );
    const offset = new THREE.Vector3(
      chunk.description.size.width * this.scale,
      chunk.description.size.width * this.scale,
      chunk.description.size.width * this.scale,
    );
    camera.position.copy(controls.target).add(offset);
    controls.update();

    // create path
    const path_offset = 0.01
    const startX = chunk.description.coordinate.x;
    const startY = chunk.description.coordinate.y;
    const width = rows;
    const height = cols;
    const path_positions = [];
    for (let i = 0; i < Math.min(width, height); i++) {
      path_positions[i] = new THREE.Vector3((startX + i) * this.scale, normalizedData[i][i] + path_offset, (startY + i) * this.scale);
    }
    const pathGeometry = new THREE.BufferGeometry().setFromPoints(path_positions);
    const pathMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
    const pathLine = new THREE.Line(pathGeometry, pathMaterial);
    const path = new THREE.CatmullRomCurve3(path_positions);
    scene.add(pathLine);

    //object to foolow path
    const objectGeometry = new THREE.SphereGeometry(10 * this.scale);
    const objectMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000 });
    const movingObject = new THREE.Mesh(objectGeometry, objectMaterial);
    movingObject.position.set(chunk.description.coordinate.x * this.scale, 0, chunk.description.coordinate.y * this.scale); // Place at center of the mesh
    scene.add(movingObject);

    this.renderer = renderer;
    this.camera = camera;
    this.scene = scene;
    this.controls = controls;

    this.path = path;
    this.movingObject = movingObject;

    this.animate();
  }

  animate = () => {
    // move object along path
    if (this.isPlaying) {
      this.tickTime += 0.001; // Adjust speed
      if (this.tickTime > 1) this.tickTime = 0; // Loop animation
      const position = this.path.getPointAt(this.tickTime);
      this.movingObject.position.set(position.x, position.y, position.z);
    }

    requestAnimationFrame(this.animate);

    this.controls.update();
    this.renderer.render(this.scene, this.camera);
  }

  togglePlay() {
    this.isPlaying = !this.isPlaying;
  }

}
