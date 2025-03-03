import { jest } from '@jest/globals';

export class OrbitControls {
  constructor() {
    this.enabled = true;
    this.enableDamping = false;
    this.dampingFactor = 0.05;
    this.target = {
      set: jest.fn()
    };
  }

  update() {
    return this;
  }

  addEventListener() {
    return this;
  }

  removeEventListener() {
    return this;
  }

  dispose() {
    return this;
  }
}