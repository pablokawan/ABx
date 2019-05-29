#pragma once

static constexpr float BASE_SPEED = 150.0f;
static constexpr float BASE_TURN_SPEED = 2000.0f;

class ClientPrediction : public LogicComponent
{
    URHO3D_OBJECT(ClientPrediction, LogicComponent);
private:
    int64_t serverTime_;
    float serverY_;
    void UpdateMove(float timeStep, uint8_t direction, float speedFactor);
    void Move(float speed, const Vector3& amount);
    void UpdateTurn(float timeStep, uint8_t direction, float speedFactor);
    void Turn(float yAngle);
    void TurnAbsolute(float yAngle);
    inline float GetSpeed(float timeElapsed, float baseSpeed, float speedFactor)
    {
        return ((timeElapsed * 1000.0f) / baseSpeed) * speedFactor;
    }
public:
    static void RegisterObject(Context* context);

    ClientPrediction(Context* context);
    ~ClientPrediction();

    /// Called on scene update, variable timestep.
    void Update(float timeStep) override;
    void FixedUpdate(float timeStep) override;
    void CheckServerPosition(int64_t time, const Vector3& serverPos);
    void CheckServerRotation(int64_t time, float rad);
};

