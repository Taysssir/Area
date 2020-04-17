#ifndef SENTINEL_SINGLETON_H
#define SENTINEL_SINGLETON_H

namespace core
{

    ////////////////////////////////////////////////////////////
    /// @brief Design to have only one global instance
    ///
    /// @example class OneInstance : public Singleton<OneInstance>
    ////////////////////////////////////////////////////////////
    template <typename T>
    class Singleton
    {

    protected:
        Singleton () { }
        Singleton(const Singleton&);
        Singleton & operator=(const Singleton&);
    public:
        static bool isInstanciated()
        {
            return (0 != Singleton<T>::singleton_);
        }

        static T *instance()
        {
            if (0 == Singleton<T>::singleton_)
            {
                singleton_ = new T;
            }
            return (static_cast<T*> (Singleton<T>::singleton_));
        }

        static void release()
        {
            if (0 != Singleton<T>::singleton_)
            {
                delete singleton_;
                singleton_ = 0;
            }
        }

    private:
        static T *singleton_;
    };

    template <typename T> T* Singleton<T>::singleton_ = 0;

}

#endif // SENTINEL_SINGLETON_H
