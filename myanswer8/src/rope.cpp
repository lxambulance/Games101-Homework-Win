#include "rope.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "CGL/vector2D.h"
#include "mass.h"
#include "spring.h"

namespace CGL {

Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass,
           float k, vector<int> pinned_nodes) {
    // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and
    // containing `num_nodes` nodes.
    assert(num_nodes >= 2);
    Vector2D delta = (end - start) / (num_nodes - 1);
    for (int i = 0; i < num_nodes; ++i) {
        masses.push_back(new Mass(delta * i + start, node_mass, false));
    }
    for (int i = 1; i < num_nodes; ++i) {
        springs.push_back(new Spring(masses[i - 1], masses[i], k));
    }
    //        Comment-in this part when you implement the constructor
    for (auto& i : pinned_nodes) {
        masses[i]->pinned = true;
    }
}

void Rope::simulateEuler(float delta_t, Vector2D gravity) {
    for (auto& s : springs) {
        // TODO (Part 2): Use Hooke's law to calculate the force on a node
        double fba = (s->m1->position - s->m2->position).norm();
        auto force = -(s->k) * (fba - s->rest_length) *
                     (s->m1->position - s->m2->position) / fba;
        s->m1->forces += force;
        s->m2->forces += -force;
    }

    float k = 0.005;
    for (auto& m : masses) {
        if (!m->pinned) {
            // TODO (Part 2): Add the force due to gravity, then compute the new
            // velocity and position
            // TODO (Part 2): Add global damping
            m->forces += gravity * m->mass;
            m->forces += -k * m->velocity;
            // for explicit
            // m->position += m->velocity * delta_t;
            m->velocity += m->forces / m->mass * delta_t;
            // for semi-implicit
            m->position += m->velocity * delta_t;
        }

        // Reset all forces on each mass
        m->forces = Vector2D(0, 0);
    }
}

void Rope::simulateVerlet(float delta_t, Vector2D gravity) {
    for (auto& s : springs) {
        // TODO (Part 3): Simulate one timestep of the rope using explicit
        // Verlet （solving constraints)
        double fab = (s->m2->position - s->m1->position).norm();
        auto force = (fab - s->rest_length) / 2 *
                     (s->m2->position - s->m1->position) / fab;
        s->m1->forces += force;
        s->m2->forces += -force;
    }

    float damping_factor = 0.00005;
    for (auto& m : masses) {
        if (!m->pinned) {
            Vector2D temp_position = m->position;
            // TODO (Part 3.1): Set the new position of the rope mass
            m->forces += gravity * m->mass;
            // auto tmp = m->position;
            // m->position =
            // m->position*2-m->last_position+m->forces/m->mass*delta_t*delta_t;
            // m->last_position = tmp;
            // TODO (Part 4): Add global Verlet damping
            auto tmp = m->position;
            m->position =
                m->position +
                (m->position - m->last_position) * (1 - damping_factor) +
                m->forces / m->mass * delta_t * delta_t;
            m->last_position = tmp;
        }
        m->forces = Vector2D(0, 0);
    }
}
}  // namespace CGL
