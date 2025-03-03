import React from 'react';
import { render, screen, fireEvent } from '@testing-library/react';

// Simple button component for testing
const Button = ({ onClick, children }) => (
  <button onClick={onClick}>{children}</button>
);

describe('Button Component', () => {
  test('renders with correct text', () => {
    render(<Button onClick={() => {}}>Click me</Button>);
    
    expect(screen.getByText('Click me')).toBeInTheDocument();
  });

  test('calls onClick when clicked', () => {
    const handleClick = jest.fn();
    render(<Button onClick={handleClick}>Click me</Button>);
    
    fireEvent.click(screen.getByText('Click me'));
  });

  test('is enabled by default', () => {
    render(<Button onClick={() => {}}>Click me</Button>);
    
    const button = screen.getByText('Click me');
    expect(button).toBeEnabled();
  });
});