#include "Player.hpp"

namespace DX12 {

    Player::Player()
        : m_moveSpeed(5.0f)
        , m_mouseSensitivity(0.002f)
        , m_movementEnabled(true)
        , m_lookEnabled(true) {

        m_transform.setPosition(0.0f, 1.0f, 0.0f);
        m_transform.setRotation(0.0f, 0.0f, 0.0f);
    }

    void Player::update(float deltaTime) {
        if (!m_movementEnabled) return;

        DirectX::XMFLOAT3 moveDelta(0.0f, 0.0f, 0.0f);
        float speed = m_moveSpeed * deltaTime;

        if (m_inputState.forward) {
            DirectX::XMFLOAT3 forward = getForward();
            moveDelta.x += forward.x * speed;
            moveDelta.z += forward.z * speed;
        }
        if (m_inputState.backward) {
            DirectX::XMFLOAT3 forward = getForward();
            moveDelta.x -= forward.x * speed;
            moveDelta.z -= forward.z * speed;
        }

        if (m_inputState.left) {
            DirectX::XMFLOAT3 right = getRight();
            moveDelta.x -= right.x * speed;
            moveDelta.z -= right.z * speed;
        }
        if (m_inputState.right) {
            DirectX::XMFLOAT3 right = getRight();
            moveDelta.x += right.x * speed;
            moveDelta.z += right.z * speed;
        }

        if (m_inputState.up) {
            moveDelta.y += speed;
        }
        if (m_inputState.down) {
            moveDelta.y -= speed;
        }

        m_transform.translate(moveDelta.x, moveDelta.y, moveDelta.z);
    }

    void Player::moveForward(float deltaTime) {
        if (!m_movementEnabled) return;
        DirectX::XMFLOAT3 forward = getForward();
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(forward.x * speed, forward.y * speed, forward.z * speed);
    }

    void Player::moveBackward(float deltaTime) {
        if (!m_movementEnabled) return;
        DirectX::XMFLOAT3 forward = getForward();
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(-forward.x * speed, -forward.y * speed, -forward.z * speed);
    }

    void Player::moveLeft(float deltaTime) {
        if (!m_movementEnabled) return;
        DirectX::XMFLOAT3 right = getRight();
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(-right.x * speed, -right.y * speed, -right.z * speed);
    }

    void Player::moveRight(float deltaTime) {
        if (!m_movementEnabled) return;
        DirectX::XMFLOAT3 right = getRight();
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(right.x * speed, right.y * speed, right.z * speed);
    }

    void Player::moveUp(float deltaTime) {
        if (!m_movementEnabled) return;
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(0.0f, speed, 0.0f);
    }

    void Player::moveDown(float deltaTime) {
        if (!m_movementEnabled) return;
        float speed = m_moveSpeed * deltaTime;
        m_transform.translate(0.0f, -speed, 0.0f);
    }

    void Player::look(float deltaX, float deltaY) {
        if (!m_lookEnabled) return;

        float yaw = deltaX * m_mouseSensitivity;
        float pitch = deltaY * m_mouseSensitivity;

        m_transform.rotateYaw(yaw);
        m_transform.rotatePitch(pitch);
    }

    void Player::setMoveSpeed(float speed) {
        m_moveSpeed = speed;
    }

    float Player::getMoveSpeed() const {
        return m_moveSpeed;
    }

    void Player::setMouseSensitivity(float sensitivity) {
        m_mouseSensitivity = sensitivity;
    }

    float Player::getMouseSensitivity() const {
        return m_mouseSensitivity;
    }

    Transform& Player::getTransform() {
        return m_transform;
    }

    const Transform& Player::getTransform() const {
        return m_transform;
    }

    DirectX::XMFLOAT3 Player::getPosition() const {
        return m_transform.getPosition();
    }

    void Player::setPosition(const DirectX::XMFLOAT3& position) {
        m_transform.setPosition(position.x, position.y, position.z);
    }

    DirectX::XMFLOAT3 Player::getRotation() const {
        return m_transform.getRotation();
    }

    void Player::setRotation(const DirectX::XMFLOAT3& rotation) {
        m_transform.setRotation(rotation.x, rotation.y, rotation.z);
    }

    DirectX::XMFLOAT3 Player::getForward() const {
        return m_transform.getForward();
    }

    DirectX::XMFLOAT3 Player::getRight() const {
        return m_transform.getRight();
    }

    DirectX::XMFLOAT3 Player::getUp() const {
        return m_transform.getUp();
    }

    DirectX::XMMATRIX Player::getViewMatrix() const {
        DirectX::XMFLOAT3 pos = m_transform.getPosition();
        DirectX::XMFLOAT3 forward = getForward();

        DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&pos);
        DirectX::XMVECTOR targetPos = DirectX::XMVectorAdd(
            eyePos,
            DirectX::XMLoadFloat3(&forward)
        );
        DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        return DirectX::XMMatrixLookAtLH(eyePos, targetPos, upVec);
    }

    DirectX::XMFLOAT4X4 Player::getViewMatrixFloat() const {
        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, getViewMatrix());
        return result;
    }

    void Player::setMovementEnabled(bool enabled) {
        m_movementEnabled = enabled;
    }

    bool Player::isMovementEnabled() const {
        return m_movementEnabled;
    }

    void Player::setLookEnabled(bool enabled) {
        m_lookEnabled = enabled;
    }

    bool Player::isLookEnabled() const {
        return m_lookEnabled;
    }

    void Player::reset() {
        m_transform.reset();
        m_transform.setPosition(0.0f, 1.0f, 0.0f);
        m_inputState = InputState();
    }

    PlayerController::PlayerController(Player* player)
        : m_player(player) {
    }

    void PlayerController::handleKeyboard(bool forward, bool backward, bool left, bool right, bool up, bool down) {
        if (!m_player) return;

        m_player->m_inputState.forward = forward;
        m_player->m_inputState.backward = backward;
        m_player->m_inputState.left = left;
        m_player->m_inputState.right = right;
        m_player->m_inputState.up = up;
        m_player->m_inputState.down = down;
    }

    void PlayerController::handleMouse(float deltaX, float deltaY) {
        if (!m_player) return;
        m_player->look(deltaX, deltaY);
    }

    void PlayerController::update(float deltaTime) {
        if (!m_player) return;
        m_player->update(deltaTime);
    }

    void Player::setMouseCapture(bool captured) {
        m_mouseCaptured = captured;

        if (m_mouseCaptured) {
            ShowCursor(false);
            RECT rect;
            GetClientRect(m_hwnd, &rect);
            m_centerX = (rect.left + rect.right) / 2;
            m_centerY = (rect.top + rect.bottom) / 2;
            SetCursorPos(m_centerX, m_centerY);
            m_firstMouse = true;
        } else {
            ShowCursor(true);
        }
    }

    void Player::handleMouseMovement(int x, int y) {
        if (!m_mouseCaptured) return;

        if (m_firstMouse) {
            m_lastMouseX = x;
            m_lastMouseY = y;
            m_firstMouse = false;
            return;
        }

        int deltaX = x - m_lastMouseX;
        int deltaY = y - m_lastMouseY;

        look(static_cast<float>(deltaX), static_cast<float>(deltaY));

        SetCursorPos(m_centerX, m_centerY);
        m_lastMouseX = m_centerX;
        m_lastMouseY = m_centerY;
    }
}