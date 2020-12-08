#pragma once
#ifndef __NPC_H__
#define __NPC_H__
#include "cgmath.h"
#include "hexgrid.h"

#define N_SPEED          200.0   
#define BIGRANGE         27.0f/4.0f

inline float randf(float m, float M) {
    float r = rand() / float(RAND_MAX - 1); return m + (M - m) * r;
}

struct NPC
{
    uint    id = 0;
    vec3   center = vec3(0);         // 3D position for translation
    float   radius = 1.f;      // radius
    //float   theta = 0.0f;            // rotation angle
    float   w1 = 0.0f, w2 = 0.0f;
    float   HP = 45.f;
    vec4   color;                  // RGBA color in [0,1]
    mat4   model_matrix;            // modeling transformation

    //void setPosition(float x, float y, float z);
    //void setSpeed(float speed) { Speed = speed; }
    //void Static();
    void update(float t);
    void if_collide_other(std::vector<NPC>& npcs);
    //void RandMove(float x1, float x2, float y, float z1, float z2, float speed);
    //void Chase(float x1, float x2, float z1, float z2, float x, float y, float z, float speed);
    //bool Found(float angle, float sight, float x, float y, float z);
};

inline std::vector<NPC> create_NPCs(uint level)
{
    std::vector<NPC> npcs;
    NPC n;

    float x, y, r;
    float w1, w2;
    float cr, cg, cb;

    for (uint i = 0; i < 2 * level; i++) {
        while (1) {
            int rand_th = int(randf(1, 7));
            float t = (PI / 3) * rand_th;
            float cost = cosf(t), sint = sinf(t);
            int rand_r = int(randf(2, 4));
            x = rand_r * sqrtf(3.f) * cost;
            y = rand_r * sqrtf(3.f) * sint;
            //r = 0.08f * abs(sin((float)rand())) + 0.05f;
            //r = sqrtf(3.f) / 20.f;
            r = 0.07f / level;
            cr = sin((float)rand());
            cg = sin((float)rand());
            cb = sin((float)rand());

            w1 = 20 * sin((float)rand()) * level;
            w2 = 20 * sin((float)rand()) * level;

            n = { i, vec3(x,y, 22.f),r,w1,w2, 45.f,vec4(cr,cg,cb, 1.0f) };

            bool collide = false;
            bool overwall = false;

            float insc = (vec2(n.center.x, n.center.y) - vec2(0, 0)).length();
            if (insc > 4.f * sqrtf(3.f) - n.radius) {
                overwall = true;
            }

            for (auto& no : npcs) {
                float dist = (n.center - no.center).length();
                if (dist < (n.radius + no.radius)) {
                    collide = true;
                    break;
                }
            }
            if (overwall == false && collide == false) break;
        }
        npcs.emplace_back(n);
    }
    for (uint i = 0; i < 3 * level; i++) {
        while (1) {
            int rand_th = int(randf(1, 7));
            float t = (PI / 3) * rand_th;
            float cost = cosf(t), sint = sinf(t);
            int rand_r = int(randf(2, 4));
            x = rand_r * sqrtf(3.f) * cost;
            y = rand_r * sqrtf(3.f) * sint;
            //r = 0.08f * abs(sin((float)rand())) + 0.05f;
            //r = sqrtf(3.f) / 20.f;
            r = 0.07f / level;
            cr = sin((float)rand());
            cg = sin((float)rand());
            cb = sin((float)rand());

            w1 = 20 * sin((float)rand()) * level;
            w2 = 20 * sin((float)rand()) * level;

            n = { i, vec3(x,y, 2.f),r,w1,w2, 45.f,vec4(cr,cg,cb, 1.0f) };

            bool collide = false;
            bool overwall = false;

            float insc = (vec2(n.center.x, n.center.y) - vec2(0, 0)).length();
            if (insc > 4.f * sqrtf(3.f) - n.radius) {
                overwall = true;
            }

            for (auto& no : npcs) {
                float dist = (n.center - no.center).length();
                if (dist < (n.radius + no.radius)) {
                    collide = true;
                    break;
                }
            }
            if (overwall == false && collide == false) break;
        }
        npcs.emplace_back(n);
    }
    for (uint i = 0; i < 4 * level; i++) {
        while (1) {
            int rand_th = int(randf(1, 7));
            float t = (PI / 3) * rand_th;
            float cost = cosf(t), sint = sinf(t);
            int rand_r = int(randf(2, 4));
            x = rand_r * sqrtf(3.f) * cost;
            y = rand_r * sqrtf(3.f) * sint;
            //r = 0.08f * abs(sin((float)rand())) + 0.05f;
            //r = sqrtf(3.f) / 20.f;
            r = 0.07f / level;
            cr = sin((float)rand());
            cg = sin((float)rand());
            cb = sin((float)rand());

            w1 = 20 * sin((float)rand()) * level;
            w2 = 20 * sin((float)rand()) * level;

            n = { i, vec3(x,y, -18.f),r,w1,w2, 45.f,vec4(cr,cg,cb, 1.0f) };

            bool collide = false;
            bool overwall = false;

            float insc = (vec2(n.center.x, n.center.y) - vec2(0, 0)).length();
            if (insc > 4.f * sqrtf(3.f) - n.radius) {
                overwall = true;
            }

            for (auto& no : npcs) {
                float dist = (n.center - no.center).length();
                if (dist < (n.radius + no.radius)) {
                    collide = true;
                    break;
                }
            }
            if (overwall == false && collide == false) break;
        }
        npcs.emplace_back(n);
    }
    return npcs;
}

inline void NPC::update(float t)
{
    //float c = cos(theta), s = sin(theta);

    mat4 scale_matrix =
    {
       radius, 0, 0, 0,
       0, radius, 0, 0,
       0, 0, radius, 0,
       0, 0, 0, 1
    };

    mat4 standup_matrix =
    {
       1, 0, 0, 0,
       0, cosf(PI / 2), -1 * sinf(PI / 2), 0,
       0, sinf(PI / 2), cosf(PI / 2), 0,
       0, 0, 0, 1
    };


    float npc_forward = atan2(w1, w2);

    mat4 rotation_matrix =
    {
       cosf(npc_forward), -1 * sinf(npc_forward), 0, 0,
       sinf(npc_forward), cosf(npc_forward), 0, 0,
       0, 0, 1, 0,
       0, 0, 0, 1
    };

    mat4 centering_matrix =
    {
        1, 0, 0, 0,
        0,1,0, 0,
        0,0,1,-0.5f,
        0,0,0,1
    };

    center.x = center.x + 0.0002f * w1;
    center.y = center.y + 0.0002f * w2;

    mat4 translate_matrix =
    {
       1, 0, 0, center.x,
       0, 1, 0, center.y,
       0, 0, 1, center.z,
       0, 0, 0, 1
    };

    model_matrix = centering_matrix * translate_matrix * rotation_matrix * standup_matrix * scale_matrix;
}

void NPC::if_collide_other(std::vector<NPC>& npcs) {
    bool self = false;

    for (auto& n : npcs) {
        if (center.x == n.center.x && center.y == n.center.y) {
            self = true;
        }
        else self = false;

        if (self == false) {
            float dist = (vec2(center.x, center.y) - vec2(n.center.x, n.center.y)).length();
            float ratio = (radius + n.radius - dist) / dist;
            float newx, newy;
            if (dist < (radius + n.radius)) {
                if (center.x >= n.center.x && center.y > n.center.y) {
                    newx = center.x + (center.x - n.center.x) * ratio;
                    newy = center.y + (center.y - n.center.y) * ratio;
                    float insc = (vec2(center.x, center.y) - vec2(0, 0)).length();
                    if (insc >= 4.f * sqrtf(3.f) - radius) {
                        n.center.x -= (center.x - n.center.x) * ratio;
                        n.center.y -= (center.y - n.center.y) * ratio;
                    }
                    else {
                        center.x = newx;
                        center.y = newy;
                    }

                }
                else if (center.x < n.center.x && center.y >= n.center.y) {
                    newx = center.x + (center.x - n.center.x) * ratio;
                    newy = center.y + (center.y - n.center.y) * ratio;
                    float insc = (vec2(center.x, center.y) - vec2(0, 0)).length();
                    if (insc >= 4.f * sqrtf(3.f) - radius) {
                        n.center.x -= (center.x - n.center.x) * ratio;
                        n.center.y -= (center.y - n.center.y) * ratio;
                    }
                    else {
                        center.x = newx;
                        center.y = newy;
                    }
                }
                else if (center.x <= n.center.x && center.y < n.center.y) {
                    newx = center.x + (center.x - n.center.x) * ratio;
                    newy = center.y + (center.y - n.center.y) * ratio;
                    float insc = (vec2(center.x, center.y) - vec2(0, 0)).length();
                    if (insc >= 4.f * sqrtf(3.f) - radius) {
                        n.center.x -= (center.x - n.center.x) * ratio;
                        n.center.y -= (center.y - n.center.y) * ratio;
                    }
                    else {
                        center.x = newx;
                        center.y = newy;
                    }
                }
                else {
                    newx = center.x + (center.x - n.center.x) * ratio;
                    newy = center.y + (center.y - n.center.y) * ratio;
                    float insc = (vec2(center.x, center.y) - vec2(0, 0)).length();
                    if (insc >= 4.f * sqrtf(3.f) - radius) {
                        n.center.x -= (center.x - n.center.x) * ratio;
                        n.center.y -= (center.y - n.center.y) * ratio;
                    }
                    else {
                        center.x = newx;
                        center.y = newy;
                    }
                }

                vec2 w = vec2(w1, w2);
                vec2 cw = vec2(n.w1, n.w2);

                vec2 new_w = w - (/*2 * n.radius **/ dot(w - cw, vec2(center.x, center.y) - vec2(n.center.x, n.center.y))) / (/*(radius + n.radius) **/ dot(vec2(center.x, center.y) - vec2(n.center.x, n.center.y), vec2(center.x, center.y) - vec2(n.center.x, n.center.y))) * (vec2(center.x, center.y) - vec2(n.center.x, n.center.y));
                vec2 new_cw = cw - (/*2 * n.radius **/ dot(cw - w, vec2(n.center.x, n.center.y) - vec2(center.x, center.y))) / (/*(radius + n.radius) **/ dot(vec2(n.center.x, n.center.y) - vec2(center.x, center.y), vec2(n.center.x, n.center.y) - vec2(center.x, center.y))) * (vec2(n.center.x, n.center.y) - vec2(center.x, center.y));

                w1 = new_w.x;
                w2 = new_w.y;
                n.w1 = new_cw.x;
                n.w2 = new_cw.y;
            }
        }
    }
}

inline void if_overwall(std::vector<NPC>& npcs) {
    for (auto& n : npcs) {
        float insc = (vec2(n.center.x, n.center.y) - vec2(0, 0)).length();
        if (insc > 9.0f * sqrtf(3.f) / 2.0f - n.radius) {
            float l = (vec2(n.w1, n.w2) - vec2(0, 0)).length();
            float alpha = atan2f(n.w2, n.w1);
            float beta = atan2f(n.center.y, n.center.x);
            float new_w1 = -1 * l * cosf(alpha - 2 * beta);
            float new_w2 = l * sinf(alpha - 2 * beta);
            n.w1 = new_w1;
            n.w2 = new_w2;
        }
    }
}
#endif