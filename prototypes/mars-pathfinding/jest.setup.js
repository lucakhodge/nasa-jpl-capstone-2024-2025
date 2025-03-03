import '@testing-library/jest-dom';
import { jest } from '@jest/globals';

global.window = {
  electronIPC: {
    openDEM: jest.fn(),
    closeDEM: jest.fn(),
    getChunk: jest.fn(),
    onDEMOpened: jest.fn(),
    onDEMClosed: jest.fn(),
    setReactLoaded: jest.fn(),
    callCpp: jest.fn()
  }
};