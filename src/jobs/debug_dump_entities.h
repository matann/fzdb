#ifndef JOBS_DEBUGDUMPENTITIES_H
#define JOBS_DEBUGDUMPENTITIES_H

#include "../session.h"
#include "./debug_job.h"
#include "../query_result.h"

class DebugDumpEntities
{

    /**
     * @brief Debugging command. Returns a list of all entities in the database, including all their properties and values.
     */
public:
    static QueryResult execute(const DebugJob &j);
};

#endif // DEBUGDUMPENTITIES_H
