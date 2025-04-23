import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { Chunk } from "../IPC/electronIPC";
import * as THREE from "three";
import { Path } from "../store/mapSlice";
export default class RegularThree {
  // canvas: HTMLCanvasElement;
  canvasRef: React.RefObject<HTMLCanvasElement>

  scale = 0.005

  renderer: THREE.WebGLRenderer;
  camera: THREE.PerspectiveCamera;
  scene: THREE.Scene;
  controls: OrbitControls;

  tickTime = 0;
  movingObject: THREE.Mesh;
  track: THREE.CatmullRomCurve3;
  isPlaying = false;

  constructor(canvasRef: React.RefObject<HTMLCanvasElement>) {
    this.canvasRef = canvasRef;
  }

  dispose() {
    this.renderer?.dispose();
  }

  resizeToCanvas() {
    const canvas = this.renderer?.domElement;
    if (!canvas || !this.camera) return;

    const width = canvas.clientWidth;   // CSS pixels only
    const height = canvas.clientHeight;

    const size = this.renderer.getSize(new THREE.Vector2());
    if (size.width !== width || size.height !== height) {
      // this.renderer.setDrawingBufferSize(this.canvasRef.current.clientWidth, this.canvasRef.current.clientHeight, window.devicePixelRatio);
      this.renderer.setSize(this.canvasRef.current.clientWidth, this.canvasRef.current.clientHeight, false);
      this.renderer.setPixelRatio(window.devicePixelRatio);
      this.camera.aspect = width / height;
      this.camera.updateProjectionMatrix();
    }
  }


  displayChunk(chunk: Chunk, path: Path) {
    // create scene, camera, and renderer
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(
      75,
      this.canvasRef.current.clientWidth / this.canvasRef.current.clientHeight,
      0.1,
      1000
    );
    const renderer = new THREE.WebGLRenderer({ canvas: this.canvasRef.current, antialias: true });
    // renderer.setDrawingBufferSize(this.canvasRef.current.clientWidth, this.canvasRef.current.clientHeight, window.devicePixelRatio);
    renderer.setSize(this.canvasRef.current.clientWidth, this.canvasRef.current.clientHeight, false);
    renderer.setPixelRatio(window.devicePixelRatio);

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap

    const rows = chunk.data.length;
    const cols = chunk.data[0].length;
    const minVal = -5000;
    const maxVal = 5000;
    // Normalize data 
    const normalizedData = chunk.data.map(
      (row) =>
        row.map((val) => (val - minVal) / (maxVal - minVal) - 1) // -.5 to .5
    );

    // create points for mesh
    const resolutionScale = 2 ** chunk.resolutionScaling;
    const positions = new Float32Array(rows * cols * 3);
    const indices = new Uint32Array((rows - 1) * (cols - 1) * 6);
    let idx3 = 0; // position index for the Float32Array
    for (let i = 0; i < rows; i += 1) {
      for (let j = 0; j < cols; j += 1) {
        positions[idx3 + 0] =
          (chunk.description.coordinate.x + j * resolutionScale) * this.scale;
        positions[idx3 + 1] = normalizedData[i][j];
        positions[idx3 + 2] =
          (chunk.description.coordinate.y + i * resolutionScale) * this.scale
        idx3 += 3;
      }
    }

    //create triangles for mesh
    // We go through each cell (i, j) and create two triangles:
    //  - Triangle A: (i,j), (i+1,j), (i,j+1)
    //  - Triangle B: (i+1,j), (i+1,j+1), (i,j+1)
    let idxi = 0; // index array pointer
    for (let i = 0; i < rows - 1; i += 1) {
      for (let j = 0; j < cols - 1; j += 1) {
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
    // const material = new THREE.MeshNormalMaterial({ flatShading: true });
    const material = new THREE.MeshStandardMaterial({
      color: 0xDB7030,
      roughness: 1,
      metalness: 0,
      flatShading: true,
    });
    // const material = new THREE.MeshStandardMaterial({
    //   color: 0xDB7030,
    //   wireframe: false,
    // });

    // Add light to make mesh look better
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    scene.add(ambientLight);

    // const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    // directionalLight.position.set(5, 5, 5);
    // // directionalLight.position.set(100, 200, 100);
    // // directionalLight.castShadow = true;
    // // directionalLight.shadow.mapSize.width = 2048;
    // // directionalLight.shadow.mapSize.width = 2048;
    // // directionalLight.shadow.mapSize.height = 2048;
    // // directionalLight.shadow.camera.near = 0.1;
    // // directionalLight.shadow.camera.far = 500;
    // // directionalLight.shadow.camera.left = -100;
    // // directionalLight.shadow.camera.right = 100;
    // // directionalLight.shadow.camera.top = 100;
    // // directionalLight.shadow.camera.bottom = -100;
    // scene.add(directionalLight);
    const directionalLight2 = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight2.position.set(chunk.description.coordinate.x * this.scale, 100, chunk.description.coordinate.y * this.scale);
    scene.add(directionalLight2);
    // const hemiLight = new THREE.HemisphereLight(0x87ceeb, 0x444422, 0.6); // Sky blue & earthy tone
    // hemiLight.position.set(0, 50, 0);
    // scene.add(hemiLight);
    const backLight = new THREE.DirectionalLight(0xffffff, 0.3);
    // backLight.position.set(-100, 100, -100);
    // scene.add(backLight);

    // const mesh = new THREE.Mesh(geometry, heatGradiant);
    const mesh = new THREE.Mesh(geometry, material);
    mesh.castShadow = true;
    mesh.receiveShadow = true;
    scene.add(mesh);

    //add controls for obiting
    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(
      (chunk.description.coordinate.x + chunk.description.dimensions.width / 2) * this.scale,
      0,
      (chunk.description.coordinate.y + chunk.description.dimensions.height / 2) * this.scale,
    );
    const offset = new THREE.Vector3(
      chunk.description.dimensions.width * this.scale,
      chunk.description.dimensions.width * this.scale,
      chunk.description.dimensions.width * this.scale,
    );
    camera.position.copy(controls.target).add(offset);
    controls.update();

    // create track from path
    const path_offset = 0.01
    const path_positions = [];
    for (let i = 0; i < path.length; i++) {
      //TODO: have a better way to get the path to confrom to teh downscaled
      // if ((path[i].y - chunk.description.coordinate.y) % resolutionScale == 0 && (path[i].x - chunk.description.coordinate.x) % resolutionScale == 0) {

      let x_pos = path[i].x * this.scale;
      let x_data_location = Math.round((path[i].y - chunk.description.coordinate.y) / resolutionScale);
      let y_data_location = Math.round((path[i].x - chunk.description.coordinate.x) / resolutionScale);
      let y_pos = normalizedData[x_data_location][y_data_location] + path_offset;
      // let y_pos = 0;
      let z_pos = path[i].y * this.scale;
      path_positions.push(new THREE.Vector3(x_pos, y_pos, z_pos));
      // }
    }
    const startGeometry = new THREE.SphereGeometry(10 * this.scale, 16, 16);
    const startMaterial = new THREE.MeshStandardMaterial({
      color: 0x228B22,
      roughness: 0.8,
      metalness: 0.1,
    }); // Green
    const startMarker = new THREE.Mesh(startGeometry, startMaterial);
    startMarker.position.copy(path_positions[0]);
    scene.add(startMarker);

    const endGeometry = new THREE.SphereGeometry(10 * this.scale, 16, 16);
    const endMaterial = new THREE.MeshStandardMaterial({
      color: 0x4682B4,
      roughness: 0.8,
      metalness: 0.1,
    }); // Blue
    const endMarker = new THREE.Mesh(endGeometry, endMaterial);
    endMarker.position.copy(path_positions[path_positions.length - 1]);
    scene.add(endMarker);

    const pathGeometry = new THREE.BufferGeometry().setFromPoints(path_positions);
    const pathMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
    const pathLine = new THREE.Line(pathGeometry, pathMaterial);
    const track = new THREE.CatmullRomCurve3(path_positions);
    scene.add(pathLine);

    //object to foolow path
    const objectGeometry = new THREE.SphereGeometry(10 * this.scale);
    const objectMaterial = new THREE.MeshStandardMaterial({
      color: 0xff0000,
      roughness: 0.8,
      metalness: 0.1,
    });
    const movingObject = new THREE.Mesh(objectGeometry, objectMaterial);
    movingObject.position.set(path_positions[0].x * this.scale, path_positions[0].y, path_positions[0].y * this.scale); // Place at center of the mesh
    scene.add(movingObject);

    this.renderer = renderer;
    this.camera = camera;
    this.scene = scene;
    this.controls = controls;

    this.track = track;
    this.movingObject = movingObject;

    this.animate();
  }

  animate = () => {
    this.resizeToCanvas();
    // move object along path
    if (this.isPlaying) {
      this.tickTime += 0.001; // Adjust speed
      if (this.tickTime > 1) this.tickTime = 0; // Loop animation
      const position = this.track.getPointAt(this.tickTime);
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
