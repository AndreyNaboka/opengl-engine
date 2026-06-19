#pragma once

class Camera;
class Window;

class DebugOverlay {
public:
  bool Init(const Window &window);
  void Render(const Camera &camera, bool freeCameraMode) const;
  void Shutdown();

private:
  bool _initialized = false;
};
