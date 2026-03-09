# Build instructions:

- Download the following OpenCV_451 folder:
https://drive.google.com/drive/folders/14SeyjbNPvsgyLKM2omcVTlTX0wAQ-_Ox?usp=sharing
and place it under ./cpp/OpenCV_451

- Open your IDE (VSCode in our case) in this folder, build and run the main to see usage example.

- ## Build and Run

This project uses **CMake** and was tested on **Windows PowerShell**.

### Build the Project

From the project root directory run:

```powershell
cd cpp
mkdir build
cd build
cmake ..
cmake --build .
```

### Run the Game

After the build finishes, run:

```powershell
.\Debug\KungFuChess.exe
```
### Example Full Session

```powershell
PS C:\Users\...\CTD25> cd cpp
PS C:\Users\...\CTD25\cpp> mkdir build
PS C:\Users\...\CTD25\cpp> cd build
PS C:\Users\...\CTD25\cpp\build> cmake ..
PS C:\Users\...\CTD25\cpp\build> cmake --build .
PS C:\Users\...\CTD25\cpp\build> .\Debug\KungFuChess.exe
```

### Output Executable

The compiled executable will be located at:
```
cpp/build/Debug/KungFuChess.exe
```
