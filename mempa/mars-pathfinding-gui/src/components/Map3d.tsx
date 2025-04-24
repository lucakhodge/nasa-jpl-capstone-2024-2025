import { useEffect, useRef, useState } from "react";
import { Chunk, Path } from "../IPC/electronIPC";
import RegularThree from "../graphics/RegularThree";
import { MyButton } from "./MyButton";
import { ChevronLeft, ChevronRight, Pause, Play } from "lucide-react";

interface Map3dPropsI {
  chunk: Chunk | null
  path: Path | null
}

export default function Map3d(props: Map3dPropsI) {
  const [regularThree, setRegularThree] = useState<RegularThree | null>(null);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const [playSpeed, setPlaySpeed] = useState(1);

  // init three on load
  useEffect(() => {
    if (canvasRef.current) {
      setRegularThree(new RegularThree(canvasRef));
    } else {
      console.log("Could not find canvasRef");
    }
  }, []);

  // load chunk if chuck appears or changes
  // Only rerender once you have new chunk
  useEffect(() => {
    if (props.chunk && props.path) {
      regularThree?.displayChunk(props.chunk, props.path);
    }
  }, [props.chunk]);

  useEffect(() => {
    regularThree?.setPlaySpeed(playSpeed);
  }, [playSpeed])

  // toggle playback of the rover along path
  const handleTogglePlay = () => {
    if (playSpeed === 0) {
      setPlaySpeed(1)
    } else {
      setPlaySpeed(0)
    }
  }
  const handleIncreasePlay = () => {
    setPlaySpeed(playSpeed + 1)
  }
  const handleDecreasePlay = () => {
    setPlaySpeed(playSpeed - 1)
  }

  return (
    <div className="w-full h-full flex flex-col">
      <div className="relative flex-grow w-full min-w-0">
        <canvas ref={canvasRef} className="absolute w-full h-full" />
      </div>
      <div className="flex w-full items-center pt-2">
        <div className="mx-auto flex items-center gap-2">
          <span className="w-10 text-center text-sm tabular-nums text-white">
          </span>
          <MyButton
            disabled={playSpeed <= -5}
            onClick={handleDecreasePlay}
            aria-label="Slower"
          >
            <ChevronLeft className="size-5" />
          </MyButton>
          <MyButton onClick={handleTogglePlay} aria-label="Play / Pause">
            {playSpeed === 0 ? (
              <Play className="size-5" />
            ) : (
              <Pause className="size-5" />
            )}
          </MyButton>
          <MyButton
            disabled={playSpeed >= 5}
            onClick={handleIncreasePlay}
            aria-label="Faster"
          >
            <ChevronRight className="size-5" />
          </MyButton>
          <span className="w-10 text-center text-sm tabular-nums text-white">
            {`${playSpeed}×`}
          </span>
        </div>
      </div>
    </div>
  );
}
