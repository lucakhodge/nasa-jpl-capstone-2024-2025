import React from 'react';
import { render } from '@testing-library/react';
import Map from '../Map3d';

jest.mock('@react-three/fiber', () => ({
  Canvas: ({ children }) => (
    <div className="w-full h-full">
      {children}
    </div>
  ),
  useThree: () => ({
    camera: { position: { set: jest.fn() } },
    scene: {},
    gl: { 
      domElement: {
        width: 800,
        height: 600,
        style: {},
        addEventListener: jest.fn(),
        removeEventListener: jest.fn(),
        getContext: jest.fn()
      }
    }
  }),
  extend: jest.fn()
}));

describe('Map Component', () => {
  test('renders container with correct classes', () => {
    const { container } = render(<Map />);
    const div = container.querySelector('.w-full.h-full');
    expect(div).toBeInTheDocument();
    expect(div).toHaveClass('w-full', 'h-full');
  });

  test('renders canvas element', () => {
    const { container } = render(<Map />);
    const canvas = container.querySelector('canvas');
    expect(canvas).toBeInTheDocument();
    expect(canvas).toHaveStyle({
      width: '100%',
      height: '100%'
    });
  });

  test('renders with initial props', () => {
    const props = {
      initialLat: 0,
      initialLng: 0,
      zoom: 2
    };
    const { container } = render(<Map {...props} />);
    const div = container.querySelector('.w-full.h-full');
    expect(div).toBeInTheDocument();
    const canvas = container.querySelector('canvas');
    expect(canvas).toBeInTheDocument();
  });
});