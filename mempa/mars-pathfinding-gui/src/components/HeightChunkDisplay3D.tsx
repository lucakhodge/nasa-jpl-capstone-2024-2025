import { OrbitControls, Plane } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { ChunkMapTile } from "../IPC/electronIPC";
import * as THREE from "three";

interface HeightChunkDisplay3DPropsI {
  chunk: ChunkMapTile;
}

export default function HeightChunkDisplay3D(
  props: HeightChunkDisplay3DPropsI
) {
  const flatData = props.chunk.data.flat();
  const minHeight = Math.min(...flatData);
  const normalizedData = flatData.map((height) => height - minHeight);
  const heightData = new Float32Array(normalizedData);

  return (
    <Canvas camera={{ position: [0, 5, 10], fov: 50 }}>
      <ambientLight intensity={0.5} />
      <directionalLight position={[10, 10, 10]} />
      {heightData && (
        <Plane args={[10, 10, 32, 32]}>
          <meshStandardMaterial wireframe />
        </Plane>
      )}
      <OrbitControls />
    </Canvas>
  );
}
