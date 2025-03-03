module.exports = {
    testEnvironment: 'jsdom',
    setupFilesAfterEnv: ['<rootDir>/jest.setup.js'],
    moduleNameMapper: {
        '\\.(css|less|scss|sass)$': 'identity-obj-proxy',
        '^three/examples/jsm/controls/OrbitControls$': '/Users/adamcarlson/nasa-jpl-capstone-2024-2025-2/prototypes/mars-pathfinding/src/__mocks__/OrbitControlsMock.js',
        '^three$': '/Users/adamcarlson/nasa-jpl-capstone-2024-2025-2/prototypes/mars-pathfinding/src/__mocks__/threeMock.js'
      },
    transform: {
      '^.+\\.(js|jsx|ts|tsx)$': ['babel-jest']
    },
    transformIgnorePatterns: [
      '/node_modules/(?!(three|@react-three)/)'
    ],
    testEnvironmentOptions: {
      url: 'http://localhost'
    }
  };