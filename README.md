# comghw1
202111337 lee dong hyun computer graphics homework1

# Description

int Width = 1280; 
int Height = 720; 
std::vector<float> OutputImage; 
**창너비와 높이, 렌더링된 이미지 데이터를 저장하는 배열을 전역변수로 설정**

class Ray { 
public: 
 vec3 origin; 
 vec3 direction; 
 Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) 
{} 
}; 
**광선을 표현하는 클래스, 광선의 시작점과 광선의 방향을 변수로 선언**

class Camera { 
public: 
 vec3 eye; 
 float l, r, b, t, d; 
 Camera(vec3 e, float left, float right, float bottom, float top, float 
depth) 
  : eye(e), l(left), r(right), b(bottom), t(top), d(depth) {} 
 
 Ray generateRay(int i, int j, int width, int height) const { 
  float u = l + (r - l) * (i + 0.5f) / width; 
  float v = b + (t - b) * (j + 0.5f) / height; 
  return Ray(eye, vec3(u, v, -d)); 
 } 
}; 
**카메라를 표현하는 클래스, 카메라의 위치 및 뷰 평면의 경계값 및 거리를 변수
로 선언, 주어진 좌표에 대해 광선을 생성하는 함수를 선언**

class Surface { 
public: 
 virtual bool intersect(const Ray& ray) const = 0; 
}; 
**모든 표면 객체의 기본 클래스**

class Sphere : public Surface { 
public: 
 vec3 center; 
 float radius; 
 Sphere(const vec3& c, float r) : center(c), radius(r) {} 
 bool intersect(const Ray& ray) const override { 
  vec3 oc = ray.origin - center; 
  float a = dot(ray.direction, ray.direction); 
  float b = 2.0f * dot(oc, ray.direction); 
  float c = dot(oc, oc) - radius * radius; 
  float discriminant = b * b - 4 * a * c; 
  return (discriminant > 0); 
 } 
}; 
**구를 표현하는 클래스, 구의 중심과 반지름 선언 및 광선과 구의 교차 여부를 판
단하는 함수 선언**

class Plane : public Surface { 
public: 
 float y; 
 Plane(float yLevel) : y(yLevel) {} 
 bool intersect(const Ray& ray) const override { 
  if (ray.direction.y == 0) return false; 
  float t = (y - ray.origin.y) / ray.direction.y; 
  return (t > 0); 
 } 
};
**평면을 표현하는 클래스, 평면의 y 위치와 광선과 평면의 교차 여부를 판단하는 
함수 생성**

class Scene { 
public: 
 std::vector<Surface*> objects; 
 void addObject(Surface* obj) { 
  objects.push_back(obj); 
 } 
 bool intersect(const Ray& ray) const { 
  for (const auto& obj : objects) { 
   if (obj->intersect(ray)) { 
    return true; 
   } 
  } 
  return false; 
 } 
};
**장면을 구성하는 클래스, 장면 내 모든 객체와 교차 여부를 판단하는 함수를 선
언**

OutputImage.clear(); 
Camera camera(vec3(0, 0, 0), -0.1f, 0.1f, -0.1f, 0.1f, 0.1f); 
Scene scene; 
**객체 선언 및 카메라 초기화**

scene.addObject(new Sphere(vec3(-4, 0, -7), 1)); 
scene.addObject(new Sphere(vec3(0, 0, -7), 2)); 
scene.addObject(new Sphere(vec3(4, 0, -7), 1)); 
scene.addObject(new Plane(-2)); 
**장면에 구와 평면 추가**

Ray ray = camera.generateRay(i, j, Width, Height);  
bool hit = scene.intersect(ray);  
vec3 color = hit ? vec3(1.0f, 1.0f, 1.0f) : vec3(0.0f, 0.0f, 0.0f); 
OutputImage.push_back(color.x); 
OutputImage.push_back(color.y); 
OutputImage.push_back(color.z);
**각 픽셀에 대해 레이 생성 및 교차 검사 수행 
픽셀별 광선 생성, 장면 내 객체와의 교차 검사 진행 
객체와 교차하면 흰색으로 처리, 그렇지 않으면 검은색으로 처리**



