#ifndef ENTITY_H
#define ENTITY_H

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "Graphics/Model.h"
#include "Graphics/Shader.h"

class Entity
{
public:
    /*Contstructors*/
    Entity(char *path, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
        : position(position), angle(angle), scale(scale)
    {
        //model = new Model(path);
        model = new Model(path);
        t_model = glm::mat4(1.0f);
        t_model = glm::translate(t_model, position);
        t_model = glm::rotate(t_model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
        t_model = glm::rotate(t_model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
        t_model = glm::rotate(t_model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
        t_model = glm::scale(t_model, scale);
    }

    /*Functions*/
    void Draw(Shader *shader, Shader *highlightShader = nullptr)
    {

        if (isTransform)
        {
            t_model = glm::mat4(1.0f);
            t_model = glm::translate(t_model, position);
            t_model = glm::rotate(t_model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
            t_model = glm::rotate(t_model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
            t_model = glm::rotate(t_model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
            t_model = glm::scale(t_model, scale);
            isTransform = false;
        }

        /*Draw*/
        shader->Use();
        shader->SetMatrix4("model", t_model);
        if (highlightShader && isSelected)
        {
            highlightShader->Use();
            highlightShader->SetMatrix4("model", glm::scale(t_model, glm::vec3(1.01, 1.01, 1.01)));
            model->Draw(shader, highlightShader);
        }
        else
            model->Draw(shader);
    }

    //Absolute rotation values
    void Rotate(glm::vec3 angle, glm::vec3 rotation)
    {
        this->angle = angle;
        this->rotation = rotation;
        isTransform = true;
    }
    //Move this entity from its current position
    void Move(glm::vec3 move)
    {
        this->position += move;
        isTransform = true;
    }
    //Scale up or down from its current position
    void Scale(glm::vec3 scale)
    {
        this->scale += scale;
        this->scale.x = glm::max(this->scale.x, 0.0f);
        this->scale.y = glm::max(this->scale.y, 0.0f);
        this->scale.z = glm::max(this->scale.z, 0.0f);
        isTransform = true;
    }

    bool isSelected = false;

private:
    bool isTransform = false;

    /*Fields*/
    glm::mat4 t_model; //transform model matrix

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 angle;
    glm::vec3 scale;

    Model *model;
};

#endif
