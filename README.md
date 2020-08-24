# Q Engine

Delving into creating a rendering engine/workflow, very much a work in progress still.

At this point, leveraging the following third-party software: 
- GLFW
- GLM
- GLAD
- Assimp
- Dear ImGui and ImGuiColorTextEdit for UI and in game shader editor
- stb_image.h


# Screenshots
Shader editor powered by ImGuiColorTextEdit
![Alt text](/screenshots/screenshot_1.png?raw=true "Shader editor powered by ImGuiColorTextEdit")
Mesh outline and modified shader to ignore camera's spotlight
![Alt text](/screenshots/screenshot_2.png?raw=true "Another frame")

# Todo

Too much to enumerate!

## Usability
- Be able to select and move around objecs 
- Create scene configurations that can be saved and restored

## Rendering
- ~~Fixed framerate~~
- Skyboxes
- Anti-aliasing
- Texture filtering

## More 
- Architecture of the engine, Entity system(?)
- Physics is not in the horizon yet
- Linux(?)