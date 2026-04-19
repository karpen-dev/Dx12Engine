#pragma once

#include "../../core/scene/transform/Transform.hpp"

#include <DirectXMath.h>

#include "core/mesh/bufferManager/BufferManager.hpp"

namespace DX12
{
    class Player
    {
    public:
        Player();

        void update(float deltaTime);

        void moveForward(float deltaTime);
        void moveBackward(float deltaTime);
        void moveLeft(float deltaTime);
        void moveRight(float deltaTime);
        void moveUp(float deltaTime);
        void moveDown(float deltaTime);

        void look(float deltaX, float deltaY);

        void setMoveSpeed(float speed);
        float getMoveSpeed() const;

        void setMouseSensitivity(float sensitivity);
        float getMouseSensitivity() const;

        Transform& getTransform();
        const Transform& getTransform() const;

        DirectX::XMFLOAT3 getPosition() const;
        void setPosition(const DirectX::XMFLOAT3& position);

        DirectX::XMFLOAT3 getRotation() const;
        void setRotation(const DirectX::XMFLOAT3& rotation);

        DirectX::XMFLOAT3 getForward() const;
        DirectX::XMFLOAT3 getRight() const;
        DirectX::XMFLOAT3 getUp() const;

        DirectX::XMMATRIX getViewMatrix() const;
        DirectX::XMFLOAT4X4 getViewMatrixFloat() const;

        void setMovementEnabled(bool enabled);
        bool isMovementEnabled() const;

        void setLookEnabled(bool enabled);
        bool isLookEnabled() const;

        bool isMouseCaptured() const { return m_mouseCaptured; }
        void setMouseCapture(bool captured);
        void handleMouseMovement(int x, int y);

        void reset();

    private:
        Transform m_transform;

        float m_moveSpeed;
        float m_mouseSensitivity;

        bool m_movementEnabled;
        bool m_lookEnabled;

        bool m_mouseCaptured;
        int m_centerX;
        int m_centerY;
        int m_lastMouseX;
        int m_lastMouseY;
        bool m_firstMouse;
        HWND m_hwnd;

        struct InputState {
            bool forward = false;
            bool backward = false;
            bool left = false;
            bool right = false;
            bool up = false;
            bool down = false;
        } m_inputState;

        friend class PlayerController;
    };

    class PlayerController {
    public:
        PlayerController(Player* player);

        void handleKeyboard(bool forward, bool backward, bool left, bool right, bool up, bool down);
        void handleMouse(float deltaX, float deltaY);
        void update(float deltaTime);

    private:
        Player* m_player;
    };
}
