#ifndef ENTITY_2CC11132_9780_4998_A779_3327320D5037
#define ENTITY_2CC11132_9780_4998_A779_3327320D5037

#include <vector>
#include <boost/smart_ptr.hpp>
#include "Database.h"

namespace Musador
{
    namespace Database
    {
        /// @class Entity
        /// @brief Represents a row in a database.
        template <typename T>
        class Entity
        {
        public:

            friend class DatabaseTest;

            /// @brief Constructor.
            /// @param[in] dbPtr Shared pointer to the IDatabase in which this Entity is stored.
            Entity(boost::shared_ptr<IDatabase> dbPtr);

            /// @brief Destructor.
            virtual ~Entity() = 0;

            /// @brief Get the numeric id of this row.
            /// @returns the numeric ID of this row.
            inline id_t getId() { return this->id; }

            /// @brief Populate this Entity instance with data from the specified row in the database.
            /// @param[in] id The numeric ID of the row in the database used to populate this Entity.
            virtual void load(id_t id);

            /// @brief Insert or Update the corresponding row in the database with the values contained in this Entity.
            /// Note that only fields which have been changed (marked dirty) will be included
            /// in the save.
            virtual void save();

            /// @brief Delete the corresponding row from the database, as specified by table name and ID.
            virtual void del();

            /// @brief Get the name of the table in which this Entity is stored
            /// Derived classes must implement this
            /// @returns The name of the table in which this Entity is stored
            /// static const wchar_t * table(); 

            template <typename F>
            boost::shared_ptr<ResultSet> getRelated(const ColumnBase& col) const;

        protected:

            void bind(ColumnBase& col);

            boost::shared_ptr<IDatabase> db;
            id_t id;
            std::vector<ColumnBase *> columns;

        };

        //////////////////////////////////////////////////////////////////////////
        // Implementations
        //////////////////////////////////////////////////////////////////////////

        template <typename T>
        Entity<T>::Entity(boost::shared_ptr<IDatabase> dbPtr) :
        db(dbPtr),
            id(NEW_ROW_ID)
        {

        }

        template <typename T>
        Entity<T>::~Entity()
        {

        }


        template <typename T> 
        void Entity<T>::load(id_t id)
        {
            this->id = id;
            std::wstringstream cond;
            cond << L"id=" << this->id;
            boost::shared_ptr<ResultSet> r = this->db->select(T::table(), this->columns,cond.str());
            if (NULL == r)
            {
                return;
            }

            int i = 0;
            for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
            {
                (*iter)->assign(*r.get(),i);
                (*iter)->setDirty(false);
                ++i;
            }
        }


        template <typename T>
        void Entity<T>::save()
        {
            // INSERT if id is NULL
            if (NEW_ROW_ID == this->id)
            {
                this->id = this->db->insert(T::table(),this->columns);
            }
            else
            {
                // otherwise UPDATE only dirty columns
                std::vector<ColumnBase *> cols;
                for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
                {
                    if ((*iter)->isDirty())
                    {
                        cols.push_back(*iter);
                    }
                }

                std::wstringstream cond;
                cond << L"id=" << this->id;
                this->db->update(T::table(),cols,cond.str());
                for (std::vector<ColumnBase *>::iterator iter = cols.begin(); iter != cols.end(); iter++)
                {
                    (*iter)->setDirty(false);
                }
            }
        }

        template <typename T>
        void Entity<T>::del()
        {
            std::wstringstream cond;
            cond << L"id=" << this->id;
            this->db->remove(T::table(), cond.str());
        }

        template <typename T>
        void Entity<T>::bind(ColumnBase& col)
        {
            this->columns.push_back(&col);
        }

        template <typename T> template <typename F>
        boost::shared_ptr<ResultSet> Entity<T>::getRelated(const ColumnBase& col) const
        {
            SQLStream q;
            q << L"SELECT " << F::table() << L".id FROM " << F::table() << L", " << T::table() << 
                L" WHERE " << F::table() << L".id = " << T::table() << L"." << this->name;
            this->db->execute(q);
        }
    }
}

#endif