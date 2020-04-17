#ifndef EVIGILANTE_SDK_SINGLETON_HPP
#define EVIGILANTE_SDK_SINGLETON_HPP

namespace eos
{
    namespace sdk
    {

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

    } // namespace sdk

} // namespace eos

#endif // EVIGILANTE_SINGLETON_HPP
