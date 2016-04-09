#ifndef FUZZY_MODEL_TYPES_CONFIDENCE
#define FUZZY_MODEL_TYPES_CONFIDENCE

#include <string>

#include "./UInt.h"
#include <iostream>

namespace model {
    namespace types {

        // Stores an integer value.
        class Confidence : public UInt {      

        public:

			Confidence() : UInt()
            {
            }
            
			Confidence(uint32_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                UInt(value, confidence, author, comment)
            {
				if (value > 100) value = 100;
				_value = value;
            }       

			void setupDefaultMetaData() override;           
            
            virtual ~Confidence() {}

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<Confidence>();
				cloned->_value = _value;
				cloned->_locked = _locked;
				cloned->_manager = _manager;
				cloned->_orderingId = _orderingId;
				return cloned;
            }          

			unsigned char confidence() const override {
				return 100;
			}

        protected:

			Confidence(const char* &serialisedData, std::size_t length) : UInt(serialisedData, length)
            {
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_AUTHORID