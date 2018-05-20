#include "core.h"

namespace console3d {
	namespace core {
		VertexTriangle::VertexTriangle() {

		}
		VertexTriangle::VertexTriangle(int i0, int i1, int i2) {
			index[0] = i0;
			index[1] = i1;
			index[2] = i2;
		}
	}
}