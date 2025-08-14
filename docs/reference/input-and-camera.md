# Input and Camera

Input (Enhanced Input)

- Actions: Move (Axis2D), Look (Axis2D), Jump (Action), Interact (Action), Sprint (Action)
- Mapping Context: `IMC_Player` (priority > 0)
- Tips: Avoid reading input in Tick; bind to actions; decouple inputs from gameplay logic via functions on character.

Camera

- SpringArm + CameraComponent attached to character; disable pawn control rotation for platformer camera.
- Follow/Cam Lag: Start with 12.0 (pos), 10.0 (rot); adjust for responsiveness vs. smoothness.
- Collision: Enable camera collision on SpringArm to avoid clipping.
- FOV: 90 (80–100 typical); reduce for tighter spaces.

Edge cases

- Controller disconnected: Ensure inputs fail gracefully.
- UI focus: Pause input mapping or adjust priority.
