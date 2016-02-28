#ifndef KEY_VALUE_INI_H
#define KEY_VALUE_INI_H

#include <stdint.h>
#include "NvAssert.h"

namespace blockchainsim
{
        // This abstract interface class is used to allow for overrides of the .INI file
        // Typically implemented as command line arguments to optionally replace settings
        // in the existing .INI file as it is being processed.
        class KeyValueOverride
        {
        public:
            virtual const char *getOverrideKeyValue(const char *section, const char *value, uint32_t index = 0) = 0;
            virtual const char *getOverrideDefine(const char *defineName) = 0;
        };

        // Call back interface to receive additional resources; used for @include  of other .INI files
        class KeyValueResource
        {
        public:
            virtual const void*   getIniResource(const char *resourceName, uint32_t &resourceLen) = 0;
            virtual void          releaseIniResource(const void *mem) = 0;
        };

        class KeyValueSection
        {
        public:
            virtual uint32_t        getKeyCount(void) const = 0;
            virtual const char      *getValue(uint32_t index, uint32_t &lineno) const = 0;
            virtual const char      *getSectionName(void) const = 0;
            virtual uint32_t        getLineNo(void) const = 0;
            virtual const char      *locateValue(const char *key, uint32_t &lineno) const = 0;
            virtual const char      *getKey(uint32_t index, uint32_t &lineno) const = 0;
            virtual void            addKeyValue(const char *key, const char *value, uint32_t lineno) = 0;
            virtual void            reset(void) = 0;
            virtual void            release(void) = 0;
            virtual void setOverrideKeyValue(KeyValueOverride *callback) = 0;
        protected:
            virtual ~KeyValueSection(void)
            {

            }
        };



        class KeyValueIni
        {
        public:
            static KeyValueIni * create(const char *fname,
                const void *data,
                uint32_t dlen,
                KeyValueOverride *callback,
                KeyValueResource *resourceCallback);


            virtual void setOverrideKeyValue(KeyValueOverride *callback) = 0;

            // if a pre-processor define is set; return it's value
            virtual const char *getDefineValue(const char *defineName) const = 0;

            // set a pre-processor define value; note this *MUST* be persistent const-char pointers!!! The pointers, not the strings are stored!!!!
            virtual void setDefineValue(const char *defineName, const char *defineValue) = 0;

            // This method let's you parse additional .INI files which can optionally override the default values
            // already declared. You can only apply as many overrides as you want; in order of precedent
            virtual void overrideINI(const char *fname, const void *mem, uint32_t dlen) = 0;
            virtual uint32_t getSectionCount(void) const = 0;
            virtual KeyValueSection * locateSection(const char *section, uint32_t &keys, uint32_t &lineno) const = 0;
            virtual const KeyValueSection * getSection(uint32_t index, uint32_t &keys, uint32_t &lineno) const = 0;

            virtual KeyValueSection  *createKeyValueSection(const char *section_name, bool reset) = 0;  // creates, or locates and existing section for editing.  If reset it true, will erase previous contents of the section.

            virtual const char *getKeyValue(const char *sectionName, const char *key) = 0;
            virtual bool getKeyValueState(const char *sectionName, const char *key) = 0;

            virtual bool save(const char *fname) const = 0;
            virtual void * saveMem(uint32_t &len) const = 0;
            virtual void releaseSaveMem(void *mem) = 0;
            virtual void reset(void) = 0;
            virtual void release(void) = 0;

            static KeyValueIni&getInstance(void)
            {
                NV_ASSERT(gInstance, "KeyValueIni instance does not exist");
                return *gInstance;
            }

            static void setInstance(KeyValueIni *instance)
            {
                gInstance = instance;
            }

        protected:
            virtual ~KeyValueIni(void)
            {

            }
            static KeyValueIni *gInstance;
        };

} // end of blockchainsim namespace

#endif // KEY_VALUE_INI_H
