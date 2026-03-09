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

### run the Project

.\Debug\KungFuChess.exe
