class input_system_subscriber
{
public:
    virtual void on_key(int code, int scancode, int action, int mods) = 0;
    virtual void on_mouse(double xpos, double ypos) = 0;
    virtual void on_scroll(double xoffset, double yoffset) = 0;
};