#include <stdint.h>
#include <gtest/gtest.h>
#include "../physics.h"

using namespace Vectormath::Aos;

struct VisualObject
{
    VisualObject() : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f), m_CollisionCount(0) {}
    Point3 m_Position;
    Quat   m_Rotation;
    int m_CollisionCount;
};

void GetWorldTransform(void* visual_object, Vectormath::Aos::Point3& position, Vectormath::Aos::Quat& rotation)
{
    if (visual_object != 0x0)
    {
        VisualObject* o = (VisualObject*) visual_object;
        position = o->m_Position;
        rotation = o->m_Rotation;
    }
    else
    {
        position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
        rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void SetWorldTransform(void* visual_object, const Vectormath::Aos::Point3& position, const Vectormath::Aos::Quat& rotation)
{
    if (!visual_object) return;
    VisualObject* o = (VisualObject*) visual_object;
    o->m_Position = position;
    o->m_Rotation = rotation;
}

class PhysicsTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_World = dmPhysics::NewWorld(Point3(-1000.0f, -1000.0f, -1000.0f), Point3(1000.0f, 1000.0f, 1000.0f), &GetWorldTransform, &SetWorldTransform);
    }

    virtual void TearDown()
    {
        dmPhysics::DeleteWorld(m_World);
    }

    dmPhysics::HWorld m_World;
};

TEST_F(PhysicsTest, Simple)
{
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));
    dmPhysics::HCollisionObject co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, 0x0);

    dmPhysics::DeleteCollisionObject(m_World, co);
    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, DynamicConstruction)
{
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));
    dmPhysics::HCollisionObject co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, 0x0);

    ASSERT_EQ((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, 0x0);

    ASSERT_NE((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, KinematicConstruction)
{
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));
    dmPhysics::HCollisionObject co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_KINEMATIC, 0x0);

    ASSERT_EQ((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_KINEMATIC, 0x0);

    ASSERT_NE((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, StaticConstruction)
{
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));
    dmPhysics::HCollisionObject co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, 0x0);

    ASSERT_EQ((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, 0x0);

    ASSERT_NE((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, TriggerConstruction)
{
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));
    dmPhysics::HCollisionObject co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_TRIGGER, 0x0);

    ASSERT_EQ((void*)0, (void*)co);

    co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_TRIGGER, 0x0);

    ASSERT_NE((void*)0, (void*)co);

    dmPhysics::DeleteCollisionObject(m_World, co);

    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, WorldTransformCallbacks)
{
    VisualObject vo;

    // Dynamic RB

    vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionShape shape = dmPhysics::NewBoxShape(Vector3(1.0f, 1.0f, 1.0f));

    dmPhysics::HCollisionObject dynamic_co = dmPhysics::NewCollisionObject(m_World, shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, &vo);

    ASSERT_EQ(0.0f, vo.m_Position.getY());
    ASSERT_EQ(0.0f, dmPhysics::GetWorldPosition(dynamic_co).getY());

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);

    ASSERT_GT(0.0f, vo.m_Position.getY());
    ASSERT_GT(0.0f, dmPhysics::GetWorldPosition(dynamic_co).getY());

    dmPhysics::DeleteCollisionObject(m_World, dynamic_co);

    // Kinematic RB

    vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionObject kinematic_co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_KINEMATIC, &vo);

    ASSERT_EQ(0.0f, vo.m_Position.getY());
    ASSERT_EQ(0.0f, dmPhysics::GetWorldPosition(kinematic_co).getY());

    vo.m_Position.setY(1.0f);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);

    ASSERT_EQ(1.0f, vo.m_Position.getY());
    ASSERT_EQ(1.0f, dmPhysics::GetWorldPosition(kinematic_co).getY());

    dmPhysics::DeleteCollisionObject(m_World, kinematic_co);

    // Static RB

    vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionObject static_co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, &vo);

    ASSERT_EQ(0.0f, vo.m_Position.getY());
    ASSERT_EQ(0.0f, dmPhysics::GetWorldPosition(static_co).getY());

    vo.m_Position.setY(1.0f);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);

    ASSERT_EQ(1.0f, vo.m_Position.getY());
    ASSERT_EQ(0.0f, dmPhysics::GetWorldPosition(static_co).getY());

    dmPhysics::DeleteCollisionObject(m_World, static_co);

    // Trigger RB

    vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionObject trigger_co = dmPhysics::NewCollisionObject(m_World, shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_TRIGGER, &vo);

    ASSERT_EQ(0.0f, vo.m_Position.getY());
    ASSERT_EQ(0.0f, dmPhysics::GetWorldPosition(trigger_co).getY());

    vo.m_Position.setY(1.0f);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);

    ASSERT_EQ(1.0f, vo.m_Position.getY());
    ASSERT_EQ(1.0f, dmPhysics::GetWorldPosition(trigger_co).getY());

    dmPhysics::DeleteCollisionObject(m_World, trigger_co);

    dmPhysics::DeleteCollisionShape(shape);
}

TEST_F(PhysicsTest, GroundBoxCollision)
{
    float ground_height_half_ext = 1.0f;
    float box_half_ext = 0.5f;

    VisualObject ground_visual_object;
    dmPhysics::HCollisionShape ground_shape = dmPhysics::NewBoxShape(Vector3(100, ground_height_half_ext, 100));
    dmPhysics::HCollisionObject ground_co = dmPhysics::NewCollisionObject(m_World, ground_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, &ground_visual_object);

    VisualObject box_visual_object;
    dmPhysics::HCollisionShape box_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject box_co = dmPhysics::NewCollisionObject(m_World, box_shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, &box_visual_object);
    dmPhysics::SetCollisionObjectInitialTransform(box_co, Point3(0, 10.0f, 0), Quat::identity());

    for (int i = 0; i < 200; ++i)
    {
        dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    }

    ASSERT_NEAR(ground_height_half_ext + box_half_ext, box_visual_object.m_Position.getY(), 0.01f);

    dmPhysics::DeleteCollisionObject(m_World, ground_co);
    dmPhysics::DeleteCollisionObject(m_World, box_co);
    dmPhysics::DeleteCollisionShape(ground_shape);
    dmPhysics::DeleteCollisionShape(box_shape);
}

void CollisionCallback(void* user_data_collider, void* user_data_collidee, void* user_data)
{
    VisualObject* vo = (VisualObject*)user_data_collider;
    ++vo->m_CollisionCount;
    int* count = (int*)user_data;
    *count = *count + 1;
}

void ContactPointCallback(const dmPhysics::ContactPoint& contact_point, void* user_data)
{
    int* count = (int*)user_data;
    *count = *count + 1;
}

TEST_F(PhysicsTest, CollisionCallbacks)
{
    float ground_height_half_ext = 1.0f;
    float box_half_ext = 0.5f;

    VisualObject ground_visual_object;
    dmPhysics::HCollisionShape ground_shape = dmPhysics::NewBoxShape(Vector3(100, ground_height_half_ext, 100));
    dmPhysics::HCollisionObject ground_co = dmPhysics::NewCollisionObject(m_World, ground_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, &ground_visual_object);

    VisualObject box_visual_object;
    dmPhysics::HCollisionShape box_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject box_co = dmPhysics::NewCollisionObject(m_World, box_shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, &box_visual_object);
    dmPhysics::SetCollisionObjectInitialTransform(box_co, Point3(0, 10.0f, 0), Quat::identity());

    int collision_count = 0;
    int contact_point_count = 0;
    for (int i = 0; i < 10; ++i)
    {
        dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
        dmPhysics::ForEachCollision(m_World, CollisionCallback, &collision_count, ContactPointCallback, &contact_point_count);
    }
    ASSERT_EQ(0, collision_count);
    ASSERT_EQ(0, contact_point_count);

    contact_point_count = 0;
    for (int i = 0; i < 200; ++i)
    {
        dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
        dmPhysics::ForEachCollision(m_World, 0x0, 0x0, ContactPointCallback, &contact_point_count);
    }
    ASSERT_GT(contact_point_count, 20);

    collision_count = 0;
    contact_point_count = 0;
    dmPhysics::ForEachCollision(m_World, CollisionCallback, &collision_count, ContactPointCallback, &contact_point_count);

    ASSERT_LT(0, box_visual_object.m_CollisionCount);
    ASSERT_LT(0, ground_visual_object.m_CollisionCount);

    ASSERT_EQ(2, collision_count); // one for each object
    ASSERT_EQ(2, contact_point_count);

    dmPhysics::DeleteCollisionObject(m_World, ground_co);
    dmPhysics::DeleteCollisionObject(m_World, box_co);
    dmPhysics::DeleteCollisionShape(ground_shape);
    dmPhysics::DeleteCollisionShape(box_shape);
}

void TriggerCollisionCallback(void* user_data_collider, void* user_data_collidee, void* user_data)
{
    VisualObject* vo = (VisualObject*)user_data_collider;
    ++vo->m_CollisionCount;
}

TEST_F(PhysicsTest, TriggerCollisions)
{
    float box_half_ext = 0.5f;

    // Test the test

    VisualObject static_vo;
    static_vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    static_vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionShape static_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject static_co = dmPhysics::NewCollisionObject(m_World, static_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, &static_vo);

    VisualObject dynamic_vo;
    dynamic_vo.m_Position = Vectormath::Aos::Point3(0.0f, 1.0f, 0.0f);
    dynamic_vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionShape dynamic_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject dynamic_co = dmPhysics::NewCollisionObject(m_World, dynamic_shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, &dynamic_vo);

    for (int i = 0; i < 20; ++i)
    {
        dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    }

    ASSERT_NEAR(1.0f, dynamic_vo.m_Position.getY(), 0.001f);
    ASSERT_EQ(0.0f, static_vo.m_Position.getY());

    dmPhysics::DeleteCollisionObject(m_World, static_co);
    dmPhysics::DeleteCollisionShape(static_shape);

    dmPhysics::DeleteCollisionObject(m_World, dynamic_co);

    // Test trigger collision: dynamic body moving into trigger

    dynamic_vo.m_Position = Vectormath::Aos::Point3(0.0f, 1.1f, 0.0f);
    dynamic_vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dynamic_co = dmPhysics::NewCollisionObject(m_World, dynamic_shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, &dynamic_vo);

    VisualObject trigger_vo;
    trigger_vo.m_Position = Vectormath::Aos::Point3(0.0f, 0.0f, 0.0f);
    trigger_vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    dmPhysics::HCollisionShape trigger_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject trigger_co = dmPhysics::NewCollisionObject(m_World, trigger_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_TRIGGER, &trigger_vo);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    dmPhysics::ForEachCollision(m_World, &TriggerCollisionCallback, 0x0, 0x0, 0x0);

    ASSERT_EQ(0, trigger_vo.m_CollisionCount);

    for (int i = 0; i < 20; ++i)
    {
        dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    }

    ASSERT_GT(1.0f - 0.1f, dynamic_vo.m_Position.getY());
    ASSERT_EQ(0.0f, trigger_vo.m_Position.getY());

    dmPhysics::ForEachCollision(m_World, &TriggerCollisionCallback, 0x0, 0x0, 0x0);
    ASSERT_LT(0, trigger_vo.m_CollisionCount);

    dmPhysics::DeleteCollisionObject(m_World, dynamic_co);
    dmPhysics::DeleteCollisionShape(dynamic_shape);

    dmPhysics::DeleteCollisionObject(m_World, trigger_co);

    // Test trigger collision: dynamic body moving into trigger

    static_vo = VisualObject();
    static_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    static_co = dmPhysics::NewCollisionObject(m_World, static_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_STATIC, &static_vo);

    trigger_vo.m_Position = Vectormath::Aos::Point3(0.0f, 1.1f, 0.0f);
    trigger_vo.m_Rotation = Vectormath::Aos::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    trigger_vo.m_CollisionCount = 0;
    trigger_co = dmPhysics::NewCollisionObject(m_World, trigger_shape, 0.0f, dmPhysics::COLLISION_OBJECT_TYPE_TRIGGER, &trigger_vo);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    dmPhysics::ForEachCollision(m_World, &TriggerCollisionCallback, 0x0, 0x0, 0x0);

    ASSERT_EQ(0, trigger_vo.m_CollisionCount);

    trigger_vo.m_Position.setY(0.8f);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    dmPhysics::ForEachCollision(m_World, &TriggerCollisionCallback, 0x0, 0x0, 0x0);

    ASSERT_LT(0, trigger_vo.m_CollisionCount);

    trigger_vo.m_CollisionCount = 0;

    trigger_vo.m_Position.setY(1.1f);

    dmPhysics::StepWorld(m_World, 1.0f / 60.0f);
    dmPhysics::ForEachCollision(m_World, &TriggerCollisionCallback, 0x0, 0x0, 0x0);

    ASSERT_EQ(0, trigger_vo.m_CollisionCount);

    dmPhysics::DeleteCollisionObject(m_World, trigger_co);
    dmPhysics::DeleteCollisionShape(trigger_shape);

    dmPhysics::DeleteCollisionObject(m_World, static_co);
    dmPhysics::DeleteCollisionShape(static_shape);
}

TEST_F(PhysicsTest, ApplyForce)
{
    float box_half_ext = 0.5f;
    dmPhysics::HCollisionShape box_shape = dmPhysics::NewBoxShape(Vector3(box_half_ext, box_half_ext, box_half_ext));
    dmPhysics::HCollisionObject box_co = dmPhysics::NewCollisionObject(m_World, box_shape, 1.0f, dmPhysics::COLLISION_OBJECT_TYPE_DYNAMIC, 0x0);
    dmPhysics::SetCollisionObjectInitialTransform(box_co, Point3(0, 10.0f, 0), Quat::identity());
    Vector3 force(1.0f, 0.0f, 0.0f);
    dmPhysics::ApplyForce(box_co, force, Point3(0.0f, 0.0f, 0.0f));
    Vector3 total_force = dmPhysics::GetTotalForce(box_co);
    ASSERT_NEAR(total_force.getX(), force.getX(), 0.01f);
    ASSERT_NEAR(total_force.getY(), force.getY(), 0.01f);
    ASSERT_NEAR(total_force.getZ(), force.getZ(), 0.01f);

    dmPhysics::DeleteCollisionObject(m_World, box_co);
    dmPhysics::DeleteCollisionShape(box_shape);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
