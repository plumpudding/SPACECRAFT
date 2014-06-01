// On enleve un warning de gcc (>2.8.*) pour glm.
// Peut-être un bug de glm ?
#pragma GCC diagnostic ignored "-Wstrict-aliasing"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_access.hpp>
