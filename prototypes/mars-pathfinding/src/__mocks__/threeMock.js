import { jest } from '@jest/globals';

const mockVector3 = {
  set: jest.fn(),
  copy: jest.fn(),
  add: jest.fn(),
  sub: jest.fn()
};

const createMockCanvas = () => {
  const canvas = document.createElement('canvas');
  canvas.setAttribute('role', 'presentation');
  return canvas;
};

export const WebGLRenderer = jest.fn(() => ({
  setSize: jest.fn(),
  render: jest.fn(),
  domElement: createMockCanvas()
}));


export const Vector3 = jest.fn(() => mockVector3);
export const Scene = jest.fn();
export const PerspectiveCamera = jest.fn();
export const Group = jest.fn();

export default {
  Vector3,
  Scene,
  WebGLRenderer,
  PerspectiveCamera,
  Group
};