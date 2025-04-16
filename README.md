# Optimization of Logistic Networks

## Algorithms and Data Structures for Complex Distribution Systems

## User Manual

### Installation

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/iamMAHAM/logistics-network-optimization
   cd logistics-network-optimization
   ```

2. **Install Dependencies**:
   Ensure you have a C compiler (e.g., GCC) and `make` installed on your system.

   - On Ubuntu/Debian:
     ```bash
     sudo apt update
     sudo apt install build-essential
     ```
   - On macOS:
     ```bash
     xcode-select --install
     ```

3. **Build the Project**:
   Run the following command to compile the project:
   ```bash
   make
   ```

### Execution

1. **Run Tests**:
   To test the algorithms on predefined datasets, use:

   ```bash
   make test
   ```

2. **Run the Main Program**:
   To execute the main program, use:

   ```bash
   ./main
   ```

3. **Generate Datasets**:
   If you need to generate new datasets, run:
   ```bash
   ./data_generator
   ```

### File Structure

- `datasets/`: Contains JSON files representing small, medium, and large networks.
- `core/`: Core data structures and graph-related functions.
- `algorithms/`: Implementations of BFS, DFS, Floyd-Warshall, TSP, and other algorithms.
- `network/`: JSON parsing and graph loading/saving utilities.
- `test_datasets.c`: Tests for algorithms using predefined datasets.

### Troubleshooting

- **Compilation Errors**:
  Ensure all dependencies are installed and the `Makefile` is correctly configured.
- **Segmentation Faults**:
  Verify the structure of the input JSON files in the `datasets/` folder.
- **Performance Issues**:
  For large datasets, consider using approximate algorithms for TSP and other NP-hard problems.

### Contact

For further assistance, please contact the development team at [support@example.com](mailto:support@example.com).
