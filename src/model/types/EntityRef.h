#ifndef FUZZY_MODEL_TYPES_ENTITY
#define FUZZY_MODEL_TYPES_ENTITY

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
	namespace types {
		class EntityRef : public Base {
		private:
			friend class TypeSerialiser;
			EHandle_t _value;
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addPrimitive(&_value, sizeof(_value));
			}
			
		public:
			EntityRef() : Base(100, 0, std::string()), _value(0)
			{
				initMemberSerialiser();				
			}

			EntityRef(EHandle_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
				Base(confidence, author, comment), _value(value)
			{
				initMemberSerialiser();
			}
			
			EntityRef(const std::string &value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
				EntityRef(std::atoll(value.c_str()), author, confidence, comment)
			{
				// Already initialised
			}
			
			virtual ~EntityRef() {}

			EHandle_t value() const { return _value; }

			virtual std::shared_ptr<Base> Clone() override {
				return std::make_shared<EntityRef>(_value, _confidence);
			}

			virtual SubType subtype() const
			{
				return SubType::TypeEntityRef;
			}

			virtual std::string logString() const override 
			{
				return std::string("EntityRef(") + std::to_string(_value) + std::string(", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			// Inherited via Base
			virtual bool Equals(const std::string &val) const override {
				return _value == std::stoll(val);
			}

			virtual std::string toString() const override {
				return std::to_string(_value);
			}

		protected:
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				//return Base::serialiseSubclass(serialiser)
				//	+ serialiser.serialise(Serialiser::SerialProperty(&_value, sizeof(EHandle_t)));
				
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialisePrimitives(serialiser);
			}

			EntityRef(const char* &serialisedData) : Base(serialisedData)
			{
				//_value = *(reinterpret_cast<const EHandle_t*>(serialisedData));
				//serialisedData += sizeof(_value);
				
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialisePrimitives(serialisedData);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_ENTITY
