#include "json_storage.h"
namespace Modules {

    void transferProperties(Serilerizeable * from, Serilerizeable * to){
        QJsonObject o;
        JsonSaveObject so(o);
        from->save(so);
        JsonLoadObject lo(o);
        to->load(lo);
    }

}
