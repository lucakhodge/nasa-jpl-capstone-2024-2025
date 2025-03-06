import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { Chunk } from "../IPC/electronIPC";
import * as THREE from "three";
import { Vector3 } from "@react-three/fiber";

export default class RegularThree {
  // Canvas element
  canvas: HTMLCanvasElement;
  
  // Scale factor for rendering
  scale = 0.005;
  
  // Store scene objects as class properties for access across methods
  private scene: THREE.Scene | null = null;
  private camera: THREE.PerspectiveCamera | null = null;
  private renderer: THREE.WebGLRenderer | null = null;
  private controls: OrbitControls | null = null;
  private normalizedData: number[][] = [];
  private chunk: Chunk | null = null;
  private dijkstraPath: THREE.Line | null = null;
  private animationId: number | null = null;

  constructor(canvas: HTMLCanvasElement) {
    this.canvas = canvas;
  }

  displayChunk(chunk: Chunk) {
    this.chunk = chunk;
    
    // Cancel any existing animation loop
    if (this.animationId !== null) {
      cancelAnimationFrame(this.animationId);
    }
    
    // Create scene
    const scene = new THREE.Scene();
    this.scene = scene;
    
    // Create camera
    const camera = new THREE.PerspectiveCamera(
      75,
      this.canvas.clientWidth / this.canvas.clientHeight,
      0.1,
      1000
    );
    this.camera = camera;
    
    // Create renderer
    const renderer = new THREE.WebGLRenderer({ canvas: this.canvas });
    renderer.setSize(this.canvas.clientWidth, this.canvas.clientHeight, false);
    this.renderer = renderer;

    // Process heightmap data
    const rows = chunk.data.length;
    const cols = chunk.data[0].length;
    const minVal = -5000;
    const maxVal = 5000;
    
    // Normalize data 
    const normalizedData = chunk.data.map(
      (row) => row.map((val) => (val - minVal) / (maxVal - minVal) - 1) // -.5 to .5
    );
    this.normalizedData = normalizedData;

    // Create points for mesh
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
    
    // Create triangles for mesh
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

    // Create mesh
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute("position", new THREE.BufferAttribute(positions, 3));
    geometry.setIndex(new THREE.BufferAttribute(indices, 1));
    // Compute normals for proper lighting
    geometry.computeVertexNormals();

    // Material for mesh
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

    // Create and add mesh to scene
    const mesh = new THREE.Mesh(geometry, material);
    scene.add(mesh);

    // Add controls for orbiting
    const controls = new OrbitControls(camera, renderer.domElement);
    this.controls = controls;
    
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

    // Create sample path (diagonal across terrain)
    const path_offset = 0.01
    const startX = chunk.description.coordinate.x;
    const startY = chunk.description.coordinate.y;
    const width = rows;
    const height = cols;
    const path_positions = [];
    for (let i = 0; i < Math.min(width, height); i++) {
      path_positions[i] = new THREE.Vector3(
        (startX + i) * this.scale, 
        normalizedData[i][i] + path_offset, 
        (startY + i) * this.scale
      );
    }
    const pathGeometry = new THREE.BufferGeometry().setFromPoints(path_positions);
    const pathMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
    const pathLine = new THREE.Line(pathGeometry, pathMaterial);
    const path = new THREE.CatmullRomCurve3(path_positions);
    scene.add(pathLine);

    // Object to follow path
    const objectGeometry = new THREE.SphereGeometry(10 * this.scale);
    const objectMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000 });
    const movingObject = new THREE.Mesh(objectGeometry, objectMaterial);
    movingObject.position.set(
      chunk.description.coordinate.x * this.scale, 
      0, 
      chunk.description.coordinate.y * this.scale
    );
    scene.add(movingObject);

    // Animation loop
    let t = 0; // Animation progress (0 to 1)
    const animate = () => {
      // Move object along path
      t += 0.001; // Adjust speed
      if (t > 1) t = 0; // Loop animation
      const position = path.getPointAt(t);
      movingObject.position.set(position.x, position.y, position.z);

      this.animationId = requestAnimationFrame(animate);
      controls.update();
      renderer.render(scene, camera);
    };
    animate();
  }

  // Method to render a path from Dijkstra's algorithm
  renderPath(pathCoordinates: Array<[number, number]>) {
    if (!this.scene || !this.chunk || !this.normalizedData.length) {
      console.warn("Cannot render path: scene or chunk not initialized");
      return;
    }

    if (pathCoordinates.length === 0) {
      console.warn("Cannot render path: empty path coordinates");
      return;
    }

    console.log("Rendering Dijkstra's path with", pathCoordinates.length, "points");

    // Remove existing Dijkstra's path if it exists
    if (this.dijkstraPath) {
      this.scene.remove(this.dijkstraPath);
    }

    const path_offset = 0.02; // Slightly higher than the default path
    const path_positions = [];

    // Create points for the path
    for (const [x, y] of pathCoordinates) {
      // Calculate the relative position in the chunk's data array
      const relX = x - this.chunk.description.coordinate.x;
      const relY = y - this.chunk.description.coordinate.y;
      
      // Get height from normalized data if coordinates are within bounds
      let height = 0;
      if (relY >= 0 && relY < this.normalizedData.length && 
          relX >= 0 && relX < this.normalizedData[0].length) {
        height = this.normalizedData[relY][relX];
      }
      
      // Create 3D vector with corrected height
      path_positions.push(new THREE.Vector3(
        x * this.scale,
        height + path_offset,
        y * this.scale
      ));
    }
    
    // Create path geometry
    const pathGeometry = new THREE.BufferGeometry().setFromPoints(path_positions);
    const pathMaterial = new THREE.LineBasicMaterial({ 
      color: 0xff0000, 
      linewidth: 3 
    });
    this.dijkstraPath = new THREE.Line(pathGeometry, pathMaterial);
    this.scene.add(this.dijkstraPath);
    
    // Focus camera on the path if controls exist
    if (this.controls && this.camera) {
      this.focusOnPath(path_positions);
    }
  }
  
  // Helper method to focus the camera on the path
  private focusOnPath(pathPoints: THREE.Vector3[]) {
    if (!this.camera || !this.controls || pathPoints.length === 0) return;
    
    // Create a bounding box around the path
    const box = new THREE.Box3().setFromPoints(pathPoints);
    const center = box.getCenter(new THREE.Vector3());
    const size = box.getSize(new THREE.Vector3());
    
    // Update controls target
    this.controls.target.copy(center);
    
    // Adjust camera position based on the path's size
    const distance = Math.max(size.x, size.y, size.z) * 1.5;
    const direction = this.camera.position.clone()
      .sub(this.controls.target)
      .normalize();
    this.camera.position.copy(center)
      .add(direction.multiplyScalar(distance));
    
    this.controls.update();
  }
}