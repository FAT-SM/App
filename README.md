# FAT-SM: Fatigue Assessment Tool for Structural Monitoring

FAT-SM is an open-source desktop application for fatigue assessment of engineering structures under cyclic loading. The software integrates classical stress-life (S-N) approaches and linear-elastic fracture mechanics (LEFM) within a unified, interactive framework.

## 📌 Purpose

Fatigue is a critical failure mechanism in engineering structures subjected to cyclic loading. The main purpose of FAT-SM is to transform stress-time data into actionable fatigue metrics, supporting both research and engineering applications in structural monitoring and reliability assessment. This is achieved by:
- Processing stress-time histories.
- Performing cycle counting.
- Evaluating fatigue damage accumulation.
- Predicting remaining fatigue life.

## ✨ Features

FAT-SM's architecture is organized into two main layers:

- **GUI:** A Qt-based user interface providing workflow-driven interaction through dedicated tabs.
- **Model:** Handles all data, validation, and fatigue computations.
- The GUI updates the Model through class setters based on the provided input data or by requesting computations. The Model performs automatic validation and consistency checks before updating its state. When modified, the Model emits notifications to the View using Qt's signal-slot mechanism, prompting the GUI to refresh, guaranteeing that the user always observes the current system state.

#### ✨ Core Features

- Project-based workflow with multiple constructional details.
- Support for two main fatigue approaches:
    - Global S-N approach where fatigue damage accumulation is computed via Miner's rule.
    - Damage-tolerant LEFM-based approach where remaining fatigue life is predicted by integrating Paris' law.
- Stress-time history processing with automatic extrema detection.
- Rainflow fatigue cycle counting.
- Interactive histograms for cycle count visualization.
- Fatigue damage computation with safety factors.
- Remaining life prediction for cracked components.
- Modular architecture for easy extension.

#### 📊 Standards

FAT-SM complies with:
- EN 1993-1-9. Eurocode 3: Design of steel structures - Part 1-9: Fatigue. CEN/TC 250, 2025.
- ASTM E1049-85. Standard Practices for Cycle Counting in Fatigue Analysis. ASTM, 2017.
- IIW-2259-15. Recommendations for Fatigue Design of Welded Joints and Components. IIW, 2016.

#### 🔧 Extending the Software

The codebase is modular:
- New fatigue approaches can be added by extending the Detail abstraction.
- Additional standards and detail types can be integrated easily.
- GUI workflows can be extended via dedicated tabs.

## 🔄 Workflow

The fatigue analysis follows a structured pipeline:
```
+--------------------------------------------+
| S-N Approach                               |
+--------------------------------------------+
|            S-N Curve Definition            |
|                     ↓                      |
|         Stress-Time History Upload         |
|                     ↓                      |
|             Extrema Detection              |
|                     ↓                      |
|          Rainflow Cycle Counting           |
|                     ↓                      |
|        Definition of Safety Factors        |
|                     ↓                      |
| Computation of Fatigue Damage Accumulation |
+--------------------------------------------+
```
```
+--------------------------------------------------+
| LEFM Approach                                    |
+--------------------------------------------------+
|         Constructional Detail Selection          |
|                         ↓                        |
|      Fatigue Crack Growth Curve Definition       |
|                         ↓                        |
|            Stress-Time History Upload            |
|                         ↓                        |
|                Extrema Detection                 |
|                         ↓                        |
|             Rainflow Cycle Counting              |
|                         ↓                        |
|        Remaining Fatigue Life Estimation         |
|                         ↓                        |
| Crack Length vs. Remaining Life Curve Generation |
+--------------------------------------------------+
```

## ⬇️ Download and Installation
No installation required. Binaries are available for Windows x64:

[Download FAT-SM Latest Version (Windows x64)](https://github.com/FAT-SM/App/releases/latest)

## 📄 License
GPLv3 - GNU General Public License v3.0

## 📖 Citation
If you use FAT-SM in your work, please cite the associated publication:

*(To be updated upon publication.)*
