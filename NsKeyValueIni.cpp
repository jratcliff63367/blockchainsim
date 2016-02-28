#include "NsKeyValueIni.h"
#include "NsStringUtils.h"
#include "NsInParser.h"
#include "NsString.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "NvAssert.h"
#include "NsUserAllocated.h"
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable:4457 4456)
#endif

using namespace blockchainsim;

namespace KEY_VALUE_INI
{
#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.

#define DEFAULT_BUFFER_SIZE 1000000
#define DEFAULT_GROW_SIZE   2000000


        class KV_FILE_INTERFACE : public blockchainsim::UserAllocated
        {
        public:
            KV_FILE_INTERFACE(const char *fname, const char *spec, void *mem, size_t len)
            {
                mMyAlloc = false;
                mRead = true; // default is read access.
                mFph = 0;
                mData = (char *)mem;
                mLen = len;
                mLoc = 0;

                if (spec && blockchainsim::stricmp(spec, "wmem") == 0)
                {
                    mRead = false;
                    if (mem == 0 || len == 0)
                    {
                        mData = (char *)NV_ALLOC(DEFAULT_BUFFER_SIZE, "KeyValueInterface");
                        mLen = DEFAULT_BUFFER_SIZE;
                        mMyAlloc = true;
                    }
                }

                if (mData == 0)
                {
                    mFph = fopen(fname, spec);
                }

                strncpy(mName, fname, 512);
            }

            ~KV_FILE_INTERFACE(void)
            {
                if (mMyAlloc)
                {
                    NV_FREE(mData);
                }
                if (mFph)
                {
                    fclose(mFph);
                }
            }

            size_t read(char *data, size_t size)
            {
                size_t ret = 0;
                if ((mLoc + size) <= mLen)
                {
                    memcpy(data, &mData[mLoc], size);
                    mLoc += size;
                    ret = 1;
                }
                return ret;
            }

            size_t write(const char *data, size_t size)
            {
                size_t ret = 0;

                if ((mLoc + size) >= mLen && mMyAlloc) // grow it
                {
                    size_t newLen = mLen + DEFAULT_GROW_SIZE;
                    if (size > newLen) newLen = size + DEFAULT_GROW_SIZE;

                    char *data = (char *)NV_ALLOC(newLen, "KeyValueInterface");
                    memcpy(data, mData, mLoc);
                    NV_FREE(mData);
                    mData = data;
                    mLen = newLen;
                }

                if ((mLoc + size) <= mLen)
                {
                    memcpy(&mData[mLoc], data, size);
                    mLoc += size;
                    ret = 1;
                }
                return ret;
            }

            size_t read(void *buffer, size_t size, size_t count)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = fread(buffer, size, count, mFph);
                }
                else
                {
                    char *data = (char *)buffer;
                    for (size_t i = 0; i < count; i++)
                    {
                        if ((mLoc + size) <= mLen)
                        {
                            read(data, size);
                            data += size;
                            ret++;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                return ret;
            }

            size_t write(const void *buffer, size_t size, size_t count)
            {
                size_t ret = 0;

                if (mFph)
                {
                    ret = fwrite(buffer, size, count, mFph);
                }
                else
                {
                    const char *data = (const char *)buffer;

                    for (size_t i = 0; i < count; i++)
                    {
                        if (write(data, size))
                        {
                            data += size;
                            ret++;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                return ret;
            }

            size_t writeString(const char *str)
            {
                size_t ret = 0;
                if (str)
                {
                    size_t len = strlen(str);
                    ret = write(str, len, 1);
                }
                return ret;
            }


            size_t  flush(void)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = fflush(mFph);
                }
                return ret;
            }


            size_t seek(size_t loc, size_t mode)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = fseek(mFph, (long)loc, int(mode));
                }
                else
                {
                    if (mode == SEEK_SET)
                    {
                        if (loc <= mLen)
                        {
                            mLoc = loc;
                            ret = 1;
                        }
                    }
                    else if (mode == SEEK_END)
                    {
                        mLoc = mLen;
                    }
                    else
                    {
                        NV_ALWAYS_ASSERT("AssertFailure");
                    }
                }
                return ret;
            }

            size_t tell(void)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = ftell(mFph);
                }
                else
                {
                    ret = mLoc;
                }
                return ret;
            }

            size_t myputc(char c)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = fputc(c, mFph);
                }
                else
                {
                    ret = write(&c, 1);
                }
                return ret;
            }

            size_t eof(void)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = feof(mFph);
                }
                else
                {
                    if (mLoc >= mLen)
                        ret = 1;
                }
                return ret;
            }

            size_t  error(void)
            {
                size_t ret = 0;
                if (mFph)
                {
                    ret = ferror(mFph);
                }
                return ret;
            }


            FILE  *mFph;
            char  *mData;
            size_t    mLen;
            size_t    mLoc;
            bool   mRead;
            char   mName[512];
            bool   mMyAlloc;

        };


        KV_FILE_INTERFACE * kvfi_fopen(const char *fname, const char *spec, void *mem = 0, size_t len = 0)
        {
            KV_FILE_INTERFACE *ret = 0;

            ret = NV_NEW(KV_FILE_INTERFACE)(fname, spec, mem, len);

            if (mem == 0 && ret->mData == 0)
            {
                if (ret->mFph == 0)
                {
                    delete ret;
                    ret = 0;
                }
            }

            return ret;
        }

        void       kvfi_fclose(KV_FILE_INTERFACE *file)
        {
            delete file;
        }

        size_t        kvfi_fread(void *buffer, size_t size, size_t count, KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->read(buffer, size, count);
            }
            return ret;
        }

        size_t        kvfi_fwrite(const void *buffer, size_t size, size_t count, KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->write(buffer, size, count);
            }
            return ret;
        }

        size_t        kvfi_fprintf(KV_FILE_INTERFACE *fph, const char *fmt, ...)
        {
            size_t ret = 0;

            char buffer[16384];
            va_list arg;
            va_start(arg, fmt);
            buffer[4095] = 0;
            blockchainsim::stringFormatV(buffer, 2047, fmt, arg);
            va_end(arg);

            if (fph)
            {
                ret = fph->writeString(buffer);
            }

            return ret;
        }


        size_t        kvfi_fflush(KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->flush();
            }
            return ret;
        }


        size_t        kvfi_fseek(KV_FILE_INTERFACE *fph, size_t loc, size_t mode)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->seek(loc, mode);
            }
            return ret;
        }

        size_t        kvfi_ftell(KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->tell();
            }
            return ret;
        }

        size_t        kvfi_fputc(char c, KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->myputc(c);
            }
            return ret;
        }

        size_t        kvfi_fputs(const char *str, KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->writeString(str);
            }
            return ret;
        }

        size_t        kvfi_feof(KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->eof();
            }
            return ret;
        }

        size_t        kvfi_ferror(KV_FILE_INTERFACE *fph)
        {
            size_t ret = 0;
            if (fph)
            {
                ret = fph->error();
            }
            return ret;
        }

        void *     kvfi_getMemBuffer(KV_FILE_INTERFACE *fph, size_t &outputLength)
        {
            outputLength = 0;
            void * ret = 0;
            if (fph)
            {
                ret = fph->mData;
                outputLength = fph->mLoc;
            }
            return ret;
        }



        class KeyValue
        {
        public:
            KeyValue(const char *key, const char *value, uint32_t lineno)
            {
                mKey = key;
                mValue = value;
                mLineNo = lineno;
            }

            const char * getKey(void) const { return mKey; };

            const char * getValue(void) const 
            { 
                return mValue; 
            };

            uint32_t getLineNo(void) const { return mLineNo; };

            void save(KV_FILE_INTERFACE *fph) const
            {
                kvfi_fprintf(fph, "%-30s = %s\n", mKey, mValue);
            }

            void setValue(const char *value)
            {
                mValue = value;
            }

        private:
            uint32_t mLineNo;
            const char *mKey;
            const char *mValue;
        };

        typedef std::vector< KeyValue > KeyValueVector;

        class KeyValueDefine
        {
        public:
            // Applies any pre-processor define modifications to this value before we return it to the caller
            virtual const char *getDefineValueChange(const char *value) = 0;
        };

        class KeyValueSectionImpl : public KeyValueSection, public blockchainsim::UserAllocated
        {
        public:
            KeyValueSectionImpl(const char *section,
                                uint32_t lineno,
                                KeyValueOverride *callback,
                                KeyValueDefine *defineCallback) : 
                                mCallback(callback)
                                , mKeyValueDefine(defineCallback)
            {
                mSection = section;
                mLineNo = lineno;
            }

            virtual uint32_t getKeyCount(void) const override
            {
                return uint32_t(mKeys.size());
            };

            virtual const char * getSectionName(void) const override
            {
                return mSection;
            };

            virtual uint32_t getLineNo(void) const override
            {
                return mLineNo;
            };

            virtual const char * locateValue(const char *key, uint32_t &lineno) const override
            {
                const char *ret = 0;

                if (mCallback)
                {
                    ret = mCallback->getOverrideKeyValue(mSection, key);
                    if (ret)
                        return ret;
                }

                for (uint32_t i = 0; i < mKeys.size(); i++)
                {
                    const KeyValue &v = mKeys[i];
                    if (blockchainsim::stricmp(key, v.getKey()) == 0)
                    {
                        ret = v.getValue();
                        lineno = v.getLineNo();
                        ret = mKeyValueDefine->getDefineValueChange(ret);
                        break;
                    }
                }
                return ret;
            }

            virtual const char *getKey(uint32_t index, uint32_t &lineno) const override
            {
                const char * ret = 0;
                if (index < mKeys.size())
                {
                    const KeyValue &v = mKeys[index];
                    ret = v.getKey();
                    lineno = v.getLineNo();
                }
                return ret;
            }

            virtual const char *getValue(uint32_t index, uint32_t &lineno) const override
            {
                const char * ret = 0;
                if (index < mKeys.size())
                {
                    const KeyValue &v = mKeys[index];
                    ret = v.getValue();
                    lineno = v.getLineNo();
                    ret = mKeyValueDefine->getDefineValueChange(ret);
                }
                return ret;
            }

            virtual void addKeyValue(const char *key, const char *value, uint32_t lineno) override
            {
                bool ret = false;

                for (uint32_t i = 0; i < mKeys.size(); i++)
                {
                    KeyValue &kv = mKeys[i];
                    if (strcmp(kv.getKey(), key) == 0)
                    {
                        kv.setValue(value);
                        ret = true;
                        break;
                    }
                }

                if (!ret)
                {
                    KeyValue kv(key, value, lineno);
                    mKeys.push_back(kv);
                    ret = true;
                }
            }

            void save(KV_FILE_INTERFACE *fph) const
            {
                if (strcmp(mSection, "@HEADER") == 0)
                {
                }
                else
                {
                    kvfi_fprintf(fph, "\n");
                    kvfi_fprintf(fph, "\n");
                    kvfi_fprintf(fph, "[%s]\n", mSection);
                }
                for (uint32_t i = 0; i < mKeys.size(); i++)
                {
                    mKeys[i].save(fph);
                }
            }


            virtual void reset(void) override
            {
                mKeys.clear();
            }

            virtual void release(void) override
            {
                delete this;
            }

            virtual void setOverrideKeyValue(KeyValueOverride *callback)
            {
                mCallback = callback;
            }


        private:
            uint32_t            mLineNo;
            const char          *mSection;
            KeyValueVector      mKeys;
            KeyValueOverride    *mCallback;
            KeyValueDefine      *mKeyValueDefine;
        };

        class DefineSpec
        {
        public:
            DefineSpec(void)
            {

            }
            DefineSpec(const char *d, const char *v) : mDefine(d), mValue(v)
            {

            }
            const char  *mDefine;
            const char  *mValue;
        };

        typedef std::vector< KeyValueSectionImpl *> KeyValueSectionVector;
        typedef std::vector< DefineSpec > DefineSpecVector;

        class KeyValueIniImpl : public KeyValueIni, public blockchainsim::InPlaceParserInterface, public blockchainsim::UserAllocated, public KeyValueDefine
        {
        public:


            typedef std::vector< blockchainsim::InPlaceParser * > InParserArray;

            KeyValueIniImpl(const char *fname, const void *mem, uint32_t len, KeyValueOverride *callback, KeyValueResource *resourceCallback) : mCallback(callback), mResourceCallback(resourceCallback)
            {
                mSkipSection = false;
                mCurrentSection = 0;
                mData.SetCommentSymbol('#');
                mData.SetCommentSymbol('!');
                mData.SetCommentSymbol(';');
                mData.SetHard('=');
                KeyValueSectionImpl *kvs = NV_NEW(KeyValueSectionImpl)("@HEADER", 0, mCallback, this);
                mSections.push_back(kvs);
                if (mem)
                {
                    char *data = (char *)NV_ALLOC(len + 1, "KeyValueIniImpl::IniFileData");
                    memcpy(data, mem, len);
                    data[len] = 0;
                    mData.SetSourceData(data, len);
                    mData.setMyAlloc(true);
                    mData.Parse(this);
                }
                else if (fname)
                {
                    mData.SetFile(fname);
                    mData.Parse(this);
                }
            }

            virtual ~KeyValueIniImpl(void)
            {
                reset();
            }

            // Applies any pre-processor define modifications tot his value before we return it to the caller
            virtual const char *getDefineValueChange(const char *ret)
            {
                if (ret)
                {
                    const char *percent = strchr(ret, '%');
                    if (percent)
                    {
                        static char convert[2048];
                        const char *src = ret;
                        char *dst = convert;
                        char *estop = &convert[2046];
                        while (*src && dst < estop)
                        {
                            if (*src == '%')
                            {
                                src++;
                                char replacement[2048];
                                char *rdst = replacement;
                                char *eos = &replacement[2046];
                                while (*src && *src != '%' && rdst < eos)
                                {
                                    *rdst++ = *src++;
                                }
                                *rdst = 0;
                                if (*src == '%')
                                {
                                    src++;
                                }
                                const char *scan = getDefineValue(replacement);
                                if (scan)
                                {
                                    while (*scan && dst < estop)
                                    {
                                        *dst++ = *scan++;
                                    }
                                }
                            }
                            else
                            {
                                *dst++ = *src++;
                            }
                        }
                        *dst = 0;
                        ret = convert;
                    }
                }
                return ret;
            }


            virtual void reset(void) override
            {
                KeyValueSectionVector::iterator i;
                for (i = mSections.begin(); i != mSections.end(); ++i)
                {
                    KeyValueSectionImpl *kvs = (*i);
                    NV_SAFE_RELEASE(kvs);
                }
                mSections.clear();
                mCurrentSection = 0;
                for (uint32_t i = 0; i < mOverrides.size(); i++)
                {
                    blockchainsim::InPlaceParser *ip = mOverrides[i];
                    delete ip;
                }
                mOverrides.clear();
            }

            virtual uint32_t getSectionCount(void) const override
            {
                return uint32_t(mSections.size());
            }

            void addDefine(const char *defineName, const char *defineState)
            {
                bool found = false;
                for (uint32_t i = 0; i < mDefines.size(); i++)
                {
                    DefineSpec &d = mDefines[i];
                    if (strcmp(d.mDefine, defineName) == 0)
                    {
                        d.mValue = defineState;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    DefineSpec d;
                    d.mDefine = defineName;
                    d.mValue = defineState;
                    mDefines.push_back(d);
                }
            }

            const char *getDefineValue(const char *defineName)
            {
                const char *ret = nullptr;

                for (uint32_t i = 0; i < mDefines.size(); i++)
                {
                    DefineSpec &d = mDefines[i];
                    if (strcmp(d.mDefine, defineName) == 0)
                    {
                        ret = d.mValue;
                        break;
                    }
                }
                return ret;
            }

            bool getDefineState(const char *defineName)
            {
                bool ret = false;

                for (uint32_t i = 0; i < mDefines.size(); i++)
                {
                    DefineSpec &d = mDefines[i];
                    if (strcmp(d.mDefine, defineName) == 0)
                    {
                        bool isTrueFalse;
                        ret = blockchainsim::isTrue(d.mValue, isTrueFalse);
                        break;
                    }
                }

                return ret;
            }

            int32_t ParseLine(int32_t lineno, int32_t argc, const char **argv, blockchainsim::SeparatorType *separatorType)  // return TRUE to continue parsing, return FALSE to abort parsing process
            {
                NV_UNUSED(separatorType);
                if (argc)
                {
                    const char *key = argv[0];
                    if (strcmp(key, "@define") == 0)
                    {
                        if (argc == 3)
                        {
                            const char *value = argv[2];
                            if (mCallback)
                            {
                                const char *ov = mCallback->getOverrideDefine(argv[1]);
                                if (ov)
                                {
                                    value = ov;
                                }
                            }
                            addDefine(argv[1], value);
                        }
                    }
                    else if (strcmp(key, "@if") == 0)
                    {
                        if (argc == 2)
                        {
                            mSkipSection = !getDefineState(argv[1]);
                        }
                    }
                    else if (strcmp(key, "@endif") == 0)
                    {
                        mSkipSection = false;
                    }
                    else if (strcmp(key, "@else") == 0)
                    {
                        mSkipSection = !mSkipSection;
                    }
                    else if (strcmp(key, "@include") == 0)
                    {
                        if (argc == 2 && mResourceCallback )
                        {
                            const char *includeName = argv[1];
                            uint32_t resourceLen;
                            const void *includeData = mResourceCallback->getIniResource(includeName, resourceLen);
                            if (includeData)
                            {
                                overrideINI(includeName, includeData, resourceLen);
                                mResourceCallback->releaseIniResource(includeData);
                            }
                        }
                    }
                    else if (key[0] == '[' && !mSkipSection)
                    {
                        key++;
                        char *scan = (char *)key;
                        while (*scan)
                        {
                            if (*scan == ']')
                            {
                                *scan = 0;
                                break;
                            }
                            scan++;
                        }
                        mCurrentSection = -1;
                        for (uint32_t i = 0; i < mSections.size(); i++)
                        {
                            KeyValueSection &kvs = *mSections[i];
                            if (blockchainsim::stricmp(kvs.getSectionName(), key) == 0)
                            {
                                mCurrentSection = (int32_t)i;
                                break;
                            }
                        }
                        //...
                        if (mCurrentSection < 0)
                        {
                            mCurrentSection = int32_t(mSections.size());
                            KeyValueSectionImpl *kvs = NV_NEW(KeyValueSectionImpl)(key, lineno, mCallback, this);
                            mSections.push_back(kvs);
                        }
                    }
                    else if (!mSkipSection)
                    {
                        const char *key = argv[0];
                        const char *value = "";
                        if (argc >= 2)
                        {
                            value = argv[1];
                            if (strcmp(value, "=") == 0 && argc >= 3)
                            {
                                value = argv[2];
                            }
                        }
                        mSections[mCurrentSection]->addKeyValue(key, value, lineno);
                    }
                }
                return 0;
            }

            virtual KeyValueSection * locateSection(const char *section, uint32_t &keys, uint32_t &lineno) const override
            {
                KeyValueSection *ret = 0;
                for (uint32_t i = 0; i < mSections.size(); i++)
                {
                    KeyValueSection *s = mSections[i];
                    if (blockchainsim::stricmp(section, s->getSectionName()) == 0)
                    {
                        ret = s;
                        lineno = s->getLineNo();
                        keys = s->getKeyCount();
                        break;
                    }
                }
                return ret;
            }

            virtual const KeyValueSection * getSection(uint32_t index, uint32_t &keys, uint32_t &lineno) const override
            {
                const KeyValueSection *ret = 0;
                if (index < mSections.size())
                {
                    const KeyValueSection &s = *mSections[index];
                    ret = &s;
                    lineno = s.getLineNo();
                    keys = s.getKeyCount();
                }
                return ret;
            }

            virtual bool save(const char *fname) const override
            {
                bool ret = false;
                KV_FILE_INTERFACE *fph = kvfi_fopen(fname, "wb");
                if (fph)
                {
                    for (uint32_t i = 0; i < mSections.size(); i++)
                    {
                        mSections[i]->save(fph);
                    }
                    kvfi_fclose(fph);
                    ret = true;
                }
                return ret;
            }

            virtual void * saveMem(uint32_t &len) const override
            {
                void *ret = 0;
                KV_FILE_INTERFACE *fph = kvfi_fopen("mem", "wmem");
                if (fph)
                {
                    for (uint32_t i = 0; i < mSections.size(); i++)
                    {
                        mSections[i]->save(fph);
                    }

                    size_t tmpLen;
                    void *temp = kvfi_getMemBuffer(fph, tmpLen);
                    len = (uint32_t)tmpLen;
                    if (temp)
                    {
                        ret = NV_ALLOC(len, "KeyValueInterface");
                        memcpy(ret, temp, len);
                    }

                    kvfi_fclose(fph);
                }
                return ret;
            }

            virtual void releaseSaveMem(void *mem) override
            {
                NV_FREE(mem);
            }


            virtual KeyValueSection  *createKeyValueSection(const char *section_name, bool reset) override  // creates, or locates and existing section for editing.  If reset it true, will erase previous contents of the section.
            {
                KeyValueSectionImpl *ret = 0;

                for (uint32_t i = 0; i < mSections.size(); i++)
                {
                    KeyValueSectionImpl *kvs = mSections[i];
                    if (strcmp(kvs->getSectionName(), section_name) == 0)
                    {
                        ret = kvs;
                        if (reset)
                        {
                            ret->reset();
                        }
                        break;
                    }
                }
                if (ret == 0)
                {
                    ret = NV_NEW(KeyValueSectionImpl)(section_name, 0, mCallback, this);
                    mSections.push_back(ret);
                }

                return static_cast<KeyValueSection *>(ret);
            }

            virtual void release(void) override
            {
                delete this;
            }

            virtual void overrideINI(const char *fname, const void *mem, uint32_t len)
            {
                blockchainsim::InPlaceParser *ipp = NV_NEW(blockchainsim::InPlaceParser);
                mOverrides.push_back(ipp);
                ipp->SetCommentSymbol('#');
                ipp->SetCommentSymbol('!');
                ipp->SetCommentSymbol(';');
                ipp->SetHard('=');
                if (mem)
                {
                    char *data = (char *)NV_ALLOC(len+1, "KeyValueIniImpl::overrideINI:fileData");
                    memcpy(data, mem, len);
                    data[len] = 0;
                    ipp->SetSourceData(data, len);
                    ipp->setMyAlloc(true);
                    ipp->Parse(this);
                }
                else if (fname)
                {
                    ipp->SetFile(fname);
                    ipp->Parse(this);
                }
            }

            virtual bool getKeyValueState(const char *sectionName, const char *key) final
            {
                bool ret = false;

                const char *value = getKeyValue(sectionName, key);
                if (value)
                {
                    bool isTrueFalse;
                    ret = blockchainsim::isTrue(value, isTrueFalse);
                }

                return ret;
            }

            virtual const char *getKeyValue(const char *sectionName, const char *key) final
            {
                const char *ret = nullptr;

                if (mCallback)
                {
                    ret = mCallback->getOverrideKeyValue(sectionName, key);
                    if (ret)
                        return ret;
                }

                uint32_t keys;
                uint32_t lineno;
                const KeyValueSection *s = locateSection(sectionName, keys, lineno);
                if (s)
                {
                    ret = s->locateValue(key, lineno);
                }


                return ret;
            }

            virtual void setOverrideKeyValue(KeyValueOverride *callback)
            {
                mCallback = callback;
                for (uint32_t i = 0; i < mSections.size(); i++)
                {
                    KeyValueSection *s = mSections[i];
                    s->setOverrideKeyValue(callback);
                }
            }

            // if a pre-processor define is set; return it's value
            virtual const char *getDefineValue(const char *defineName) const final override
            {
                const char *ret = nullptr;

                for (uint32_t i = 0; i < mDefines.size(); ++i)
                {
                    const DefineSpec &d = mDefines[i];
                    if (strcmp(defineName, d.mDefine) == 0)
                    {
                        ret = d.mValue;
                    }
                }

                return ret;
            }

            // set a pre-processor define value
            virtual void setDefineValue(const char *defineName, const char *defineValue) final override
            {
                bool found = false;
                for (uint32_t i = 0; i < mDefines.size(); ++i)
                {
                    DefineSpec &d = mDefines[i];
                    if (strcmp(defineName, d.mDefine) == 0)
                    {
                        d.mValue = defineValue;
                        found = true;
                    }
                }
                if (!found)
                {
                    DefineSpec d(defineName, defineValue);
                    mDefines.push_back(d);
                }
            }


        private:
            KeyValueOverride            *mCallback;
            KeyValueResource            *mResourceCallback;
            int32_t                     mCurrentSection;
            KeyValueSectionVector       mSections;
            blockchainsim::InPlaceParser       mData;
            InParserArray               mOverrides;
            DefineSpecVector            mDefines;
            bool                        mSkipSection;
        };
}

using namespace KEY_VALUE_INI;

namespace blockchainsim
{
        KeyValueIni *KeyValueIni::gInstance = nullptr;

        KeyValueIni * KeyValueIni::create(const char *fname, const void *data, uint32_t dlen, KeyValueOverride *callback,KeyValueResource *resourceCallback)
        {
            KeyValueIniImpl *ret = NV_NEW(KeyValueIniImpl)(fname, data, dlen, callback,resourceCallback);
            return static_cast<KeyValueIni *>(ret);
        }

} // end of blockchainsim namespace
