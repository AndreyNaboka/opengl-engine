#include <string>
#include <memory>

class texture 
{
public:
   static std::shared_ptr<texture> create(const std::string& name, const std::string& path);   
   bool load();
   void bind();
   ~texture();
      
private:
   texture(const std::string& name, const std::string& path);

private:
   bool _is_loaded = false;
   std::string _path;
   std::string _name;
};