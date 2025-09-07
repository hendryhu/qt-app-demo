Note: This is a archived mirror of the original project repository.

# COMP3004 Final Project

This is the repository for our team's final project for COMP 3004, where we are building a simulation of the t:slim Insulin Pump in Qt.

## Repository Structure

The diagrams and documentation for this project can be found in our [Google Drive](https://drive.google.com/drive/folders/1C4y4gvbcMegKVWl2U0bCg_5ZITpmZFi2). You can also find them in the `diagrams` directory inside this repo.

The code can be found in the [`QtProject`](</QtProject>) directory. To compile:
1. Open up [`QtProject.pro`](/QtProject/QtProject.pro) in QtCreator in the 3004 class VM.
2. Go to the per-project settings and set the build path to wherever you'd like. This can be done from the "Projects" button on the left side of QtCreator with a wrench icon. **If you don't set the build path, the project will not compile.**
3. Build and run the project.

The project plan with our revised schedule and brief explanation for delay can be found in [`Project Plan.pdf`](</Project Plan.pdf>) in the root directory of this repo.

## Team Contributions (so far): 

Hendry: 
- Main Window UI + Hardware toggles/inputs
- HomeScreen UI + Logic
- Popups UI + Logic
- Graph Enhancements
- Warning/Error Handling
- State machine diagrams
- Data singleton class (model)
- Traceability Matrix

SeWon: 
- Logging UI + Logic.
- Main Success Scenario Sequence Diagrams
- Connect Bolus to Home Screen
- Traceability Matrix

Daniel: 
- Graph UI + Logic
- Instant/Extended Bolus Delivery UI + Logic
- Control IQ Implementation
- Video  
- Traceability Matrix

Ashwin: 
- Activity Screen UI + Logic
- Settings Menu and Submenus UI + Logic (mypump screen, controliq screen)
- Safety Scenario Sequence Diagrams
- UML Class Diagram
- Control IQ Integration with Activity 
- Traceability Matrix

Teja: 
- Settings Menu UI + Logic (personal profile CRUD)
- Personal Profiles UI + Logic
- Use Case Model
- Use Case Diagram
- Activity UI + Logic
- Traceability Matrix
