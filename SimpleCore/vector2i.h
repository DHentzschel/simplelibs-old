#ifndef VECTOR2I_H
#define VECTOR2I_H

#include "vector2.h"

/**
 * Extends the template class Vector2 by type int32.
 *
 * @author Daniel Hentzschel on 11.05.2019.
 */
class SIMPLECORE_API Vector2i : public Vector2<int> {
public:
	/**
	 * Sets the x and y values to zero.
	 */
	Vector2i() :
		Vector2<int>()
	{}

	/**
	 * Copies the x and y values from parameter vector.
	 *
	 * @param vector the copyable vector
	 */
	Vector2i(const Vector2i& vector) :
		Vector2<int>(vector)
	{}

	/**
	 * Copies the x and y value from parameters.
	 *
	 * @param x the new x value
	 * @param y the new y value
	 */
	Vector2i(const int x, const int y) :
		Vector2<int>(x, y)
	{}
};

#endif // VECTOR2I_H
