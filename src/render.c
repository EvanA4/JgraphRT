#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tga.h"
#include "render.h"

#define PI 3.1415926535F


typedef struct Ray {
    Vec3 pos;
    Vec3 dir;
} Ray;


// helper functions for handling vectors and matrices
static float vlen(Vec3 vec) {
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

static void vnorm(Vec3 vec) {
    float len = vlen(vec);
    if (len) for (int i = 0; i < 3; ++i) vec[i] = vec[i] / len;
}

static void vcross(Vec3 a, Vec3 b, Vec3 dest) {
    dest[0] = a[1] * b[2] - a[2] * b[1];
    dest[1] = a[2] * b[0] - a[0] * b[2];
    dest[2] = a[0] * b[1] - a[1] * b[0];
}

static void mulM4V4(Mat4 mat, Vec4 vec, Vec4 dest) {
    for (int i = 0; i < 4; ++i) {
        // for each entry in destination...
        dest[i] = 0.0F;

        for (int j = 0; j < 4; ++j) {
            // add each operation
            dest[i] += mat[i][j] * vec[j];
        }
    }
}

static void subV3(Vec3 a, Vec3 b, Vec3 dest) {
    for (int i = 0; i < 3; ++i) dest[i] = a[i] - b[i];
}

static float dotV3(Vec3 a, Vec3 b) {
    float out = 0.0F;
    for (int i = 0; i < 3; ++i) out += a[i] * b[i];
    return out;
}


// actual rendering functions
static Ray *create_ray(Renderer *rptr, int px) {
    // thanks to https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
    // also thanks http://www.codinglabs.net/article_world_view_projection_matrix.aspx

    // determine screen space of pixel
    int pxx = px % rptr->width;
    int pxy = px / rptr->width;
    float ndcx = (pxx + .5F) / (float) rptr->width;
    float ndcy = (pxy + .5F) / (float) rptr->height;
    // printf("\n\n[%d] NRC Space: %.2f, %.2f\n", px, nrcx, nrcy);
    float scrnx = 2 * ndcx - 1;
    float scrny = 1 - 2 * ndcy;
    // printf("[%d] Screen Space: %.2f, %.2f\n", px, scrnx, scrny);

    // determine camera space of pixel
    float aspect = rptr->width / (float) rptr->height;
    float tana = tan(rptr->fov / 360.0F * PI);
    float camx = scrnx * aspect * tana;
    float camy = scrny * tana;
    // printf("[%d] Camera Space: %.2f, %.2f\n", px, camx, camy);

    Vec4 camSpace = {camx, camy, 1, 1};

    // get direction of ray
    Ray *out = malloc(sizeof(Ray));
    Vec4 worldSpace4 = {0.0F, 0.0F, 0.0F, 0.0F};
    mulM4V4(rptr->view, camSpace, worldSpace4);
    Vec3 worldSpace3 = {0.0F, 0.0F, 0.0F};
    for (int i = 0; i < 3; ++i) {
        out->pos[i] = rptr->pos[i];
        worldSpace3[i] = worldSpace4[i];
    }
    // printf("[%d] World Space: %.2f, %.2f, %.2f\n", px, worldSpace3[0], worldSpace3[1], worldSpace3[2]);

    Vec3 direction = {0.0F, 0.0F, 0.0F};
    subV3(worldSpace3, out->pos, direction);
    vnorm(direction);

    for (int i = 0; i < 3; ++i) out->dir[i] = direction[i];

    return out;
}


Renderer *render_init(KerrArgs *args) {
    Renderer *out = malloc(sizeof(Renderer));
    for (int i = 0; i < 3; ++i) {
        out->pos[i] = args->pos[i];
        out->dir[i] = args->dir[i];
    }
    out->fov = args->fov;
    out->width = args->width;
    out->height = args->height;
    out->scene = args->scene;

    // compute view matrix
    //     thanks to https://www.3dgep.com/understanding-the-view-matrix/
    Vec3 forward = {args->dir[0], args->dir[1], args->dir[2]};
    Vec3 position = {args->pos[0], args->pos[1], args->pos[2]};
    for (int i = 0; i < 3; ++i) out->view[i][2] = forward[i];

    //     cross product between cam's forward (dir) and true Y => cam's right vector
    Vec3 right = {0.0F, 0.0F, 0.0F};
    Vec3 trueY = {0.0F, 1.0F, 0.0F};
    vcross(forward, trueY, right);
    vnorm(right);
    for (int i = 0; i < 3; ++i) out->view[i][0] = right[i];

    //     cross product between cam's right and cam's forward => cam's up vector
    Vec3 up = {0.0F, 0.0F, 0.0F};
    vcross(right, forward, up);
    vnorm(up);
    for (int i = 0; i < 3; ++i) out->view[i][1] = up[i];

    //     set position of camera
    out->view[3][3] = 1.0F;
    for (int i = 0; i < 3; ++i) out->view[i][3] = position[i];

    return out;
}


// sphere rendering functions
static void pierce_atm(Ray *cur, Vec2 dest) {
    // returns (distance to sphere, length of path through sphere)
    // credit to Sebastian Lague at https://www.youtube.com/watch?v=DxfEbulyFcY&t=154s
    Vec3 spherePos = {0.0F, 0.0F, 0.0F};
    float sphereR = 2.0F;

    // solve parameterized equation for sphere collision https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection
    Vec3 originDiff = {0.0F, 0.0F, 0.0F};
    subV3(cur->pos, spherePos, originDiff);
    float b = 2. * dotV3(cur->dir, originDiff);
    float c = dotV3(originDiff, originDiff) - sphereR * sphereR;
    float disc = b * b - 4. * c;

    if (disc > 0.0F) {
        float sqrtDisc = sqrt(disc);
        float distNearIntersect = ((-b - sqrtDisc) / 2.0F) > 0.0F ? ((-b - sqrtDisc) / 2.0F) : 0.0F; // max for in case camera is inside sphere
        float distFarIntersect = (-b + sqrtDisc) / 2.0F;
        float rawAtmDepth = distFarIntersect - distNearIntersect;

        if (distFarIntersect >= 0.0F) {
            dest[0] = distNearIntersect;
            dest[1] = rawAtmDepth;
            return;
        }
    }
    
    dest[0] = 1000000.0F;
    dest[1] = 0.0F;
}


static Pixel render_sphere(Ray *cur) {
    // check for ray collision with sphere
    Vec2 info = {0.0F, 0.0F};
    pierce_atm(cur, info);
    Vec3 nearint = {0.0F, 0.0F, 0.0F};
    for (int i = 0; i < 3; ++i) {
        nearint[i] = (cur->pos[i] + cur->dir[i] * info[0]) * 1000.0F;
        if (nearint[i] < 0.0F) nearint[i] = 0.0F;
        else if (nearint[i] > 1.0F) nearint[i] = 1.0F;
    }

    // determine color of pixel from collision info
    Pixel out;
    if (info[1]) {
        // if collided, render surface of sphere
        info[1] /= 4.0F;
        if (info[1] < 0.0F) info[1] = 0.0F;
        else if (info[1] > 1.0F) info[1] = 1.0F;

        out.r = (unsigned char) 255 * nearint[0] * info[1];
        out.g = (unsigned char) 255 * nearint[1] * info[1];
        out.b = (unsigned char) 255 * nearint[2] * info[1];

    } else {
        // otherwise, just render the background
        Vec3 finalPos = {0.0F, 0.0F, 0.0F};
        for (int i = 0; i < 3; ++i) {
            finalPos[i] = cur->pos[i] + cur->dir[i] * 10000.0F;
            if (finalPos[i] > 1.0F) finalPos[i] = 1.0F;
            else if (finalPos[i] < 0.0F) finalPos[i] = 0.0F;
        }
        out.r = (unsigned char) 255 * finalPos[0];
        out.g = (unsigned char) 255 * finalPos[1];
        out.b = (unsigned char) 255 * finalPos[2];
    }

    
    return out;
}


// black hole rendering functions
static void get_ea(float L, Vec2 ep, Vec2 dest) { // Returns the second derivative of s
    float ep_len = sqrt(ep[0] * ep[0] + ep[1] * ep[1]);
    float c = -1.5F * (L * L) / pow(ep_len, 5.0F);
    dest[0] = ep[0] * c;
    dest[1] = ep[1] * c;
}


static void get_finalpos(Ray *cur, Vec3 dest) {
    // Initialize constants, ehat0, ehat1 ep, ev
    int N = 3750; // 750
    float dt = .01F; // .05F
    Vec2 ep = {vlen(cur->pos), 0.0F};
    Vec3 ehat0 = {
        cur->pos[0] / ep[0],
        cur->pos[1] / ep[0],
        cur->pos[2] / ep[0]
    };
    float dot = dotV3(cur->dir, ehat0);
    Vec3 ehat1 = {
        cur->dir[0] - dot * ehat0[0],
        cur->dir[1] - dot * ehat0[1],
        cur->dir[2] - dot * ehat0[2]
    };
    vnorm(ehat1);
    Vec2 ev = {dotV3(cur->dir, ehat0), dotV3(cur->dir, ehat1)};
    float L = ep[0] * ev[1];
    float diskSlope = -ehat0[1] / ehat1[1];

    for (int i = 0; i < N; ++i) {
        // Euler's method
        Vec2 step_ep = {ev[0] * dt, ev[1] * dt};
        Vec2 step_ev = {0.0F, 0.0F};
        get_ea(L, ep, step_ev);
        step_ev[0] *= dt; step_ev[1] *= dt;

        // Update variables
        Vec2 old_ep = {ep[0], ep[1]};
        ep[0] += step_ep[0];
        ep[1] += step_ep[1];
        ev[0] += step_ev[0];
        ev[1] += step_ev[1];

        // Photon entered event horizon, return black
        if (sqrt(ep[0] * ep[0] + ep[1] * ep[1]) < 1.0F) {
            for (int i = 0; i < 3; ++i) dest[i] = 0.0F;
            return;
        }

        // Photon hit accretion disk
        if (((ep[0] * diskSlope) < ep[1]) != ((old_ep[0] * diskSlope) < old_ep[1])) {
            float current_m = (ep[1] - old_ep[1]) / (ep[0] - old_ep[0]);
            float current_b = old_ep[1] - current_m * old_ep[0];
            float cross_e0 = -current_b / (diskSlope - current_m);
            float cross_e1 = diskSlope * cross_e0;
            Vec3 finalPos = {
                cross_e0 * ehat0[0] + cross_e1 * ehat1[0],
                cross_e0 * ehat0[1] + cross_e1 * ehat1[1],
                cross_e0 * ehat0[2] + cross_e1 * ehat1[2],
            };
            float final_len2 = cross_e0 * cross_e0 + cross_e1 * cross_e1;

            if (final_len2 > 9.0F && final_len2 < 36.0F) {
                for (int i = 0; i < 3; ++i) dest[i] = finalPos[i];
                return;
            }
        }
    }

    Vec2 finalep = {ep[0] + 1000.0F * ev[0], ep[1] + 1000.0F * ev[1]};
    Vec3 finalPos = {
        finalep[0] * ehat0[0] + finalep[1] * ehat1[0],
        finalep[0] * ehat0[1] + finalep[1] * ehat1[1],
        finalep[0] * ehat0[2] + finalep[1] * ehat1[2],
    };

    for (int i = 0; i < 3; ++i) dest[i] = finalPos[i]; // Assume photon is far enough away from black hole to travel in straight line
}


static Pixel render_schwarz(Ray *cur) {
    // determine final position of photon
    Vec3 finalPos = {0.0F, 0.0F, 0.0F};
    get_finalpos(cur, finalPos);
    float finalLen = vlen(finalPos);

    // determine final color of pixel
    Pixel out;
    if (finalLen > 100.0F) { // if photon didn't hit disk
        for (int i = 0; i < 3; ++i) {
            if (finalPos[i] > 1.0F) finalPos[i] = 1.0F;
            else if (finalPos[i] < 0.0F) finalPos[i] = 0.0F;
        }
        out.r = (unsigned char) 255 * finalPos[0];
        out.g = (unsigned char) 255 * finalPos[1];
        out.b = (unsigned char) 255 * finalPos[2];

    } else if (finalLen > 2.9F) { // photon hit accretion disk
        float fphi = (finalPos[2] >= 0.0F ? 1.0F : -1.0F) * acos(finalPos[0] / sqrt(finalPos[0] * finalPos[0] + finalPos[2] * finalPos[2])) + PI;
        float r01 = (finalLen - 3.0F) / 3.0F;
        float fphi01 = (fphi) / PI / 2.;
        fphi01 -= (int) fphi01;
        out.r = (unsigned char) 255 * r01;
        out.g = (unsigned char) 255 * fphi01;
        out.b = (unsigned char) 0;

    } else { // photon entered black hole
        out.r = (unsigned char) 0;
        out.g = (unsigned char) 0;
        out.b = (unsigned char) 0;
    }

    return out;
}


Pixel render(Renderer *rptr, int px) {
    Ray *cur = create_ray(rptr, px);
    Pixel out = {(unsigned char) 0, (unsigned char) 0, (unsigned char) 0};
    if (!strcmp(rptr->scene, "schwarz")) {
        out = render_schwarz(cur);
    } else if (!strcmp(rptr->scene, "sphere")) {
        out = render_sphere(cur);
    }
    free(cur);
    return out;
}