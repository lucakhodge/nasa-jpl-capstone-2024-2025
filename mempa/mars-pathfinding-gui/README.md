This document should introduce how the app works and how to get it running on your machine. If you run into problems or the instructions are unclear, please let me know.

# Running the app

## Basic installs and running

NOTE: for windows the installations should be done in native windows, not WSL (It is possible to use WSL, but it is more difficult and we were not able to figure it out).

### Install npm

Depends on your operating system and package manager

- i.e. for Mac: `brew install node`

### Install yarn

```
npm install --global yarn
```

### Run

Navigate into `prototypes/mars-pathfinding` and run the bellow command (if a `node_modules/` or `yarn.lock` file already exist you may need to delete them):

```
yarn install
```

Then run :

```
yarn start
```

This should pop up a window

You may have libraries missing:  install them and try again.

## Running C++ code

Currently, you need to make the binary for the platform.

Navigate `prototypes/example-cpp` and use `make` command to create you binary (Make sure if you are on windows this is done through windows and not WSL). This will generate an `example` executable. that should be put in `prototypes/mars-pathfinding/bin` (replace old version of `example` if needed).

So, yes, this is annoying and not ideal for now. Later we will have a better way to bundle executables from different platforms.

## Verify everything is working

`yarn start` should launch a window. The UI is not good right now. But you should be able to select a .tiff file, end coordinates and get a chunk to display (try a ~1000x1000 area). **If you enter a large area it may crash, currently it does not check or handle for too large portions of the DEM. Also I think it matters the order you enter coordinate (start should be smaller and end larger).** You should be able to orbit around the 3D terrain at this point. If you set up the C++ code, you should be able to click the "RUN CPP" button and get back a hello message.

It is not very robust at this point, so there really isn't a user experience right now. The main thing is to get the basic functionality working. Let me know if something is not working or if you need help

## Making

**Not tested on platforms other than macOS, feel free to skip this part for now**

```
yarn make
```

This will create a directory `out/`. Generated executables will be found here. Again, I have not verified on other operating systems, so may not work for you platform.

# Technologies used

This is just a list to give an overview of what we are working with, I can go in more detail with what each are and how to use if you want.

- Electron: <https://www.electronjs.org/>
  - For creating desktop app
- Electron Forge: <https://www.electronforge.io/>
  - For building and distributing our app
- Typescript: <https://www.typescriptlang.org/>
  - Javascript with types
- React: <https://react.dev/>
  - For creating an interactive UI
- Tailwind: <https://tailwindcss.com/>
  - For css styling
- Redux: <https://redux.js.org/>, <https://react-redux.js.org/>
  - State management
- three.js: <https://threejs.org/>
  - 3D rendering
- geotiff.js: <https://geotiffjs.github.io/>
  - Reading DEM data

# File structure information

This will explain how the app is structured and where too look if you are developing.

## Yarn files

(you shouldn't need touch these in general, but if you are having problems with starting the project you may need to remove `yarn.lock` and `node_modules`)

- `package.json` defines which packages are used in the project
- `yarn.lock` what versions of packages were actually installed
- `node_modules` contains the installed packages themselves

## src file tree

```
src
├── App.tsx
├── IPC
│   ├── DEMManager.ts
│   ├── electronIPC.ts
│   ├── mainProcDEMFileHandlers.ts
│   └── runCpp.ts
├── components
│   ├── DemListener.tsx
│   ├── FigmaPage.tsx
│   ├── FileStatus.tsx
│   ├── HeightChunkDisplay.tsx
│   ├── HeightChunkDisplay3D.tsx
│   ├── LoadFileButton.tsx
│   ├── Map3d.tsx
│   ├── MyButton.tsx
│   ├── MyNumberInput.tsx
│   ├── RunCpp.tsx
│   └── SampleThree.tsx
├── graphics
│   └── RegularThree.ts
├── index.css
├── main.ts
├── output.css
├── pages
│   └── PathfindingPage.tsx
├── preload.ts
├── renderer.ts
├── store
│   ├── demSlice.ts
│   ├── hooks.ts
│   ├── paramatersSlice.ts
│   └── store.ts
└── types
    ├── electron-squirrel-startup.d.ts
    └── electron.d.ts
```

- `src/App.tsx` is the root for the frontend
  - Needs to have `<Provider>` and `<DemListener />` to have redux and DEM IPC respectively work correctly
    - If you don't understand this, don't worry, just don't remove them.
  - This will have the top most react component in it
- `src/components/`
  - This contains all the frontend react components
    - If you are working on the UI layout, this is pretty much where everything lives
- `src/graphics/`
  - This is currently where the three.js code lives, handling rending the 3D map
- `src/store/`
  - Defines the redux store for state
- `src/IPC/`
  - Contains code pertaining to Inter-Process Communication for the Electron app
    - Basically, there is the main process that has access to the file system and whatnot. And there is the render process which actually displays the UI. The two need to send data back and forth.
- Other `src/`
  - `src/main.ts` and `src/preload.ts` are for setting up electron processes and IPC.
  - `src/types/` contains some type definitions to make Typescript behave nicely

# Original set up

This section can be ignored unless you to see what all was used to set up the app.

**TODO: I NEED TO GO BACK AND DOCUMENT THIS LATER**
