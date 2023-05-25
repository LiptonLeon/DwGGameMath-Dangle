#include "dwgSimpleGraphics.h"

// entry point for the app (using WinMain, so no console appears, just the rendering window)
int WinMain()
{
	// init window and rendering with given width, height, and title of the window
	if (!dwgInitApp(1600, 900, "Dangle"))
		return 1;

    // Ball chain (dangle)
    constexpr int num = 11;
    Vector3 pos[num] = {};
    Vector3 vel[num] = {};
    float d[num - 1] = {};
    float radius = 0.5f;

    // Red collider ball
    Vector3 colPos = {0.2f, 0.f, -3.f};
    float colRadius = 2.0f;

    // Physics stuff
    const Vector3 g = {0.0f, 0.0f, -150.0f};
    float deltaTimeLeft = 0.0f;

    for(int i = 0; i < num; ++i)
    {
        vel[i] = Vector3(0.0f);
        pos[i] = Vector3(0.0f, 2.0f - (float)i, 0.0f);
    }

    for(int i = 0; i < num - 1; ++i)
    {
        d[i] = length(pos[i] - pos[i + 1]);
    }

	// main game loop, each iteration is a single frame
	while (!dwgShouldClose())
	{
		const double globalTime = dwgGlobalTime();	// global time - time since the start of the app
		const float deltaTime = dwgDeltaTime();			// delta time - time since last frame

        deltaTimeLeft += deltaTime;

        const float dt_fixed_step = 1.0f / 60.0f;
        while (deltaTimeLeft > dt_fixed_step)
        {
            deltaTimeLeft +=- dt_fixed_step;
            float dt = dt_fixed_step;

            Vector3 newPos[num] = {};

            // Update velocity and position
            for (int i = 1; i < num; ++i)
            {
                vel[i] += g * dt; // Apply gravity
                vel[i] *= 0.97f - (float)i * 0.03f;  // Apply damp (simulate air friction)
                vel[i] += Vector3((float)i * 0.5f * (sinf((float)globalTime) * sinf((float)globalTime * 2.f) + (float)1),
                                  (float)i * 0.3f * (cosf((float)globalTime * 4.f) * cosf((float)globalTime * 4.f) * sinf((float)globalTime* 1.f)),
                                  0.f); // Apply some wind
                newPos[i] = pos[i] + vel[i] * dt;
            }

            // Resolve constrains
            newPos[0] = Vector3(0.0f, 0.0f, 2.0f);
            colPos = {cosf(0.1f * (float)globalTime) * 15.0f, 0.f, -1.f};

            // Resolve collision with obstacle
            const int iterations = 4;
            for(int it = 0; it < iterations; ++it)
            {
                for (int i = 0; i < num - 1; ++i)
                {
                    // Resolve distance constrains
                    const Vector3 diff = normalize(newPos[i + 1] - newPos[i]) * d[i];
                    newPos[i + 1] = newPos[i] + diff;

                    // Resolve collision
                    const Vector3 colDiff = newPos[i+1] - colPos;
                    const float colDistance = length(colDiff);
                    const float overlap = radius + colRadius - colDistance;
                    if (overlap > 0.0f)
                    {
                        const Vector3 separation = normalize(colDiff) * overlap;
                        newPos[i + 1] += separation;
                    }
                }
            }

            // Write final velocity and position
            for (int i = 0; i < num; ++i)
            {
                vel[i] = (newPos[i] - pos[i]) / dt;
                pos[i] = newPos[i];
            }
        }

        // Draw balls
        for (int i = 0; i < num; ++i)
        {
            dwgDebugSphere(pos[i], Vector3(radius), Vector3(0.0f, 0.f + 0.1f * (float)i, 1.0f));
        }

        dwgDebugSphere(colPos, Vector3(colRadius), Vector3(1.0f, 0.f, 0.f));


        // prepare camera
		const Point3 eye = { 10.0f, 10.0f, 2.0f };				// eye position
		const Point3 at = { 0.0f, 0.0f, 0.0f };					// what we're looking at
		const Vector3 up = { 0.0f, 0.0f, 1.0f };				// where's the up direction for camera
		const Matrix4 camera = Matrix4::lookAt(eye, at, up);	// create camera view matrix
		const float fov = 120.0f;								// field of view - angle of view of the camera (for perspective)

		// draw scene
		dwgRender(camera, fov);
	}

	// release rendering and window app
	dwgReleaseApp();

	return 0;
}
