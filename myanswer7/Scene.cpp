//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray& ray) const {
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection& pos, float& pdf) const {
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum) {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(const Ray& ray, const std::vector<Object*>& objects,
                  float& tNear, uint32_t& index, Object** hitObject) {
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

Vector3f Scene::shader(Intersection& pos, Vector3f wo) const {
    Vector3f result;

    Intersection pos_light;
    float pdf_light;
    sampleLight(pos_light, pdf_light);

    Vector3f ws = normalize(pos_light.coords - pos.coords);
    auto intersectws = Scene::intersect(Ray(pos.coords, ws));
    if (!intersectws.happened || intersectws.obj->hasEmit())
        result += pos_light.emit * pos.m->eval(wo, ws, pos.normal) *
                  dotProduct(ws, pos.normal) *
                  dotProduct(-ws, pos_light.normal) /
                  sqrDistance(pos.coords, pos_light.coords) / pdf_light;

    if (get_random_float() < RussianRoulette) {
        Vector3f wi = pos.m->sample(wo, pos.normal);
        auto intersectwi = Scene::intersect(Ray(pos.coords, wi));
        if (intersectwi.happened && !intersectwi.obj->hasEmit())
            result += shader(intersectwi, -wi) *
                      pos.m->eval(wo, wi, pos.normal) *
                      dotProduct(wi, pos.normal) /
                      pos.m->pdf(wo, wi, pos.normal) / RussianRoulette;
    }

    return result;
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray& ray, int depth) const {
    // TO DO Implement Path Tracing Algorithm here
    Vector3f result;
    Intersection intersection = Scene::intersect(ray);

    if (intersection.happened) {
        if (intersection.obj->hasEmit())
            result = intersection.m->getEmission();
        else
            result = shader(intersection, -ray.direction);
    }
    return result;
}
