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

    const geometry = new THREE.BoxGeometry(1, 1, 1);
    const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
    const cube = new THREE.Mesh(geometry, material);
    scene.add(cube);

    camera.position.z = 5;

    function animate() {
      renderer.render(scene, camera);
      cube.rotation.x += 0.01;
      cube.rotation.y += 0.01;
    }
    renderer.setAnimationLoop(animate);
  }
}
