import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { ChunkMapTile } from "../IPC/electronIPC";
import * as THREE from "three";
export default class RegularThree {
  scene: THREE.Scene;
  camera: THREE.PerspectiveCamera;
  renderer: THREE.WebGLRenderer;

  constructor(canvas: HTMLCanvasElement) {
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(
      75,
      // window.innerWidth / window.innerHeight,
      canvas.clientWidth / canvas.clientHeight,
      0.1,
      1000
    );

    const renderer = new THREE.WebGLRenderer({ canvas: canvas });
    // renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setSize(canvas.clientWidth, canvas.clientHeight, false);
    // document.body.appendChild(renderer.domElement);

    // const geometry = new THREE.BoxGeometry(1, 1, 1);
    // const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
    // const cube = new THREE.Mesh(geometry, material);
    // scene.add(cube);

    camera.position.z = 10;

    // function animate() {
    //   renderer.render(scene, camera);
    //   cube.rotation.x += 0.01;
    //   cube.rotation.y += 0.01;
    // }
    // renderer.setAnimationLoop(animate);

    renderer.render(scene, camera);

    this.scene = scene;
    this.camera = camera;
    this.renderer = renderer;
  }

  displayChunk(chunk: ChunkMapTile) {
    while (this.scene.children.length > 0) {
      this.scene.remove(this.scene.children[0]);
    }
    console.log(chunk);
    const rows = chunk.data.length;
    const cols = chunk.data[0].length;

    const minVal = chunk.data.reduce(
      (min, row) =>
        Math.min(
          min,
          row.reduce((rowMin, val) => Math.min(rowMin, val), Infinity)
        ),
      Infinity
    );
    const maxVal = chunk.data.reduce(
      (max, row) =>
        Math.max(
          max,
          row.reduce((rowMax, val) => Math.max(rowMax, val), -Infinity)
        ),
      -Infinity
    );
    // Normalize data to range [-1, 1]
    const normalizedData = chunk.data.map((row) =>
      row.map((val) => 2 * ((val - minVal) / (maxVal - minVal)) - 1)
    );

    // 2. Create typed arrays
    // Each vertex has (x, y, z) => 3 floats
    const positions = new Float32Array(rows * cols * 3);

    // Each cell has two triangles = 6 indices
    const indices = new Uint32Array((rows - 1) * (cols - 1) * 6);

    // 3. Fill the position buffer
    let idx3 = 0; // position index for the Float32Array
    const scale = 0.1; // Adjust this value to make the mesh larger or smaller
    const xOffset = -((cols - 1) * scale) / 2;
    const zOffset = -((rows - 1) * scale) / 2;
    for (let i = 0; i < rows; i++) {
      for (let j = 0; j < cols; j++) {
        // x = j, z = i, y = heightData[i][j]
        positions[idx3 + 0] = j * scale + xOffset; // x
        positions[idx3 + 1] = normalizedData[i][j]; // y
        positions[idx3 + 2] = i * scale + zOffset; // z

        idx3 += 3;
      }
    }

    // 4. Fill the index buffer
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

    // 5. Build the BufferGeometry
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute("position", new THREE.BufferAttribute(positions, 3));
    geometry.setIndex(new THREE.BufferAttribute(indices, 1));

    // (optional) compute normals for proper lighting
    geometry.computeVertexNormals();

    // 6. Create the mesh
    const material = new THREE.MeshStandardMaterial({
      color: 0x88ee88,
      wireframe: false,
    });

    // Add some light to see the mesh better
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    this.scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(5, 5, 5);
    this.scene.add(directionalLight);

    // Adjust camera position to better view the terrain
    this.camera.position.set(5, 5, 5);
    this.camera.lookAt(0, 0, 0);

    const mesh = new THREE.Mesh(geometry, material);
    this.scene.add(mesh);

    // const geometry = new THREE.BoxGeometry(1, 1, 1);
    // const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
    // const cube = new THREE.Mesh(geometry, material);
    // this.scene.add(cube);

    this.renderer.render(this.scene, this.camera);

    const r = this.renderer;
    const c = this.camera;
    const s = this.scene;

    const controls = new OrbitControls(this.camera, this.renderer.domElement);

    function animate() {
      requestAnimationFrame(animate);
      // mesh.rotation.x += 0.01;
      // mesh.rotation.y += 0.01;
      controls.update();
      r.render(s, c);
    }
    animate();
  }
}
