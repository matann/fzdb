#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <string>

#include "model/types/Int.h"
#include "model/types/String.h"
#include "model/types/Date.h"
#include "model/types/EntityRef.h"
#include "model/types/SubType.h"
#include "model/Entity.h"
#include "model/EntityManager.h"

#include "model/TypeSerialiser.h"
#include "model/Serialiser.h"
#include "model/EntitySerialiser.h"

class SerialisationTest : public ::testing::Test
{
protected:
    virtual void setUp() {

    }
    virtual void TearDown() {

    }
};

void testSerialisation(const BasePointer &typePtr)
{
    Serialiser ser;
    model::types::TypeSerialiser tSer(typePtr);
    std::size_t serialisedLength = tSer.serialise(ser);
    std::size_t advance = 0;
    BasePointer unserialised = model::types::TypeSerialiser::unserialise(ser.begin(), &advance);

    EXPECT_EQ(true, unserialised->memberwiseEqual(typePtr.get()));
    EXPECT_EQ(advance, serialisedLength);
}

std::shared_ptr<Entity> createSampleEntity()
{
    using namespace model::types;

    std::shared_ptr<Entity> ent(new Entity(0, 1));
    EntityProperty* propInt = new EntityProperty(1, model::types::SubType::TypeInt32);
    EntityProperty* propString = new EntityProperty(2, model::types::SubType::TypeString);
    EntityProperty* propDate = new EntityProperty(3, model::types::SubType::TypeDate);
    EntityProperty* propEntityRef = new EntityProperty(4, model::types::SubType::TypeEntityRef);

    propInt->append(BasePointer(new Int(1234, 42, 98, "Integer comment")));
    propInt->append(BasePointer(new Int(5678, 24, 89, "Integer comment")));

    propString->append(BasePointer(new String("Serialise this! @#!@$@%", 23, 1, "String comment")));
    propString->append(BasePointer(new String("Mew Purrymore", 32, 2, "I for one welcome our new robot overlords.")));

    propDate->append(BasePointer(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::After, 12, "Date comment")));
    propDate->append(BasePointer(new Date(Date::StructuredDate(1210, 4, 4), 1, Date::Ordering::Before, 13, "Date comment")));

    propEntityRef->append(BasePointer(new EntityRef((EHandle_t)1, 123785, 99, "Entity ref comment")));
    propEntityRef->append(BasePointer(new EntityRef((EHandle_t)5674564, 2, 34, "Entity ref comment")));

    // Entity takes ownership of properties here.
    ent->insertProperty(std::shared_ptr<EntityProperty>(propInt));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propString));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propDate));
    ent->insertProperty(std::shared_ptr<EntityProperty>(propEntityRef));

    return ent;
}

// Test that values of different types serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseValues)
{
    using namespace model::types;

    testSerialisation(BasePointer(new Int(1234, 42, 98, "Integer comment")));
    testSerialisation(BasePointer(new String("Serialise this! @#!@$@%", 32, 1, "String comment")));
    testSerialisation(BasePointer(new Date(Date::StructuredDate(1150, 3, 21), 1, Date::Ordering::EqualTo, 12, "Date comment")));
    testSerialisation(BasePointer(new EntityRef((EHandle_t)1, 123785, 99, "Entity ref comment")));
}

// Test that entities with different numbers of properties serialise and unserialise correctly.
TEST_F(SerialisationTest, testSerialiseEntities)
{
    using namespace model::types;

    std::shared_ptr<Entity> ent = createSampleEntity();

    Serialiser serialiser;
    EntitySerialiser entSer(ent);
    entSer.serialise(serialiser);
    std::shared_ptr<Entity> unserialised = EntitySerialiser::unserialise(serialiser.begin(), serialiser.size());

    EXPECT_EQ(true, ent->memberwiseEqual(unserialised.get()));
}

// Test that entity manager with different numbers of entities serialises and unserialises correctly.
TEST_F(SerialisationTest, testSerialiseEntityManager)
{
    EntityManager manager;

    EXPECT_EQ(true, false);
}

// Check that files are written to and read from disk correctly.
TEST_F(SerialisationTest, testWriteReadFiles)
{

    EXPECT_EQ(true, false);
}
