#pragma once

struct UnitInfo
{
    UnitInfo(const char* _name, float _radius, float _attackRadius, float _speed, float _yawSpeed,
            float _maxHealth, float _damage, float _attackSpeed, bool _canMoveWhileAttacking,
            const char* _selectionSound, const char* _attackSound)
    {
        name = _name;
        radius = _radius;
        attackRadius = _attackRadius;
        speed = _speed;
        yawSpeed = _yawSpeed;
        maxHealth = _maxHealth;
        damage = _damage;
        attackSpeed = _attackSpeed;
        canMoveWhileAttacking = _canMoveWhileAttacking;
        selectionSound = _selectionSound;
        attackSound = _attackSound;
    }

    const char* name;

    float radius;
    float attackRadius;
    float speed;
    float yawSpeed; //in degrees

    float maxHealth;
    float damage;
    float attackSpeed; //the time one attack takes
    bool canMoveWhileAttacking;

    const char* selectionSound;
    const char* attackSound;
};

extern UnitInfo infoForUnitType[];
