import QtQuick 2.12
import QtQml.Models 2.12

ListModel{
    property QtObject null_section: QtObject{}
    property ListModel sections: ListModel{
    }

    function addSection(name, offered){
        append({"name":name, "offered": offered, "section" : null_section});
        sections.append(get(count-1));
    }
    function changeSection(index, newSection){
        if(index<0||index>=count)
            return;
        if(get(index).section === null_section)
            return;
        if(get(index).section === newSection)
            return;
        for(var i = 0; i<count;++i){
            if(get(i) === newSection){
                move(index,i+1)
                break;
            }
        }
    }
    function removeEntry(index){
        if(index>=0&&index<count){
            if(get(index).section === null_section){
                if(index === count-1 || get(index+1).section === null_section){
                    for(var i = 0; i < sections.count ; ++i){
                        if(sections.get(i) === get(index)){
                            sections.remove(i);
                            break;
                        }
                    }
                    remove(index);
                }
            }else{
                remove(index);
            }
        }
    }

    function addEntryAtIndex(index){
        //find section
        var section = null;
        for(var i = index;i>=0;--i){
            if(get(i).section === null_section){
                section = get(i);
                break;
            }
        }
        if(section){
            insert(index+1, {"name": "Neues Getränk", "description":"","zutaten":"","price":"","section":section,"offered":true})
        }
    }

    function addEntry(name, description, zutaten, price, section, offered){
        if(section === null || section === undefined){
            throw "Illegal arguments";
        }
        //find index to insert:
        var insertIndex = -1;
        for(var i = 0; i < count; ++i){
            if(get(i) === section){
                for(; i < count; ++i){
                    if(get(i).section === null_section){
                        insertIndex = i-1;
                        break;
                    }
                }
                if(insertIndex === -1){
                    insertIndex = count-1;
                }
                break;
            }
        }
        if(insertIndex === -1){
            throw "Parent section does not exists!";
        }
        insert(insertIndex,{"name": name, "description":description,"zutaten":zutaten,"price":price,"section":section,"offered":offered});
    }
    function moveOnePosition(index,up){
        if(index>0 && index < count){
            if(get(index).section !== null_section){
                var dir = up ? 1 : -1;
                if(up&&index<=1 || !up&&index>=count-1)
                    return;
                if(get(index+dir).section === get(index).section){
                    move(index,index+dir,1);
                }
            }else{
                var to = index + dir;
                while(to >= 0 && to < count && !get(to).section){
                    to += dir;
                }
                if(to === index + dir)
                    return;
                var objectsToMove = 1;
                var i = index + 1;
                while(i<count && !get(i).section){
                    ++i;
                    ++objectsToMove;
                }
                move(index,to,objectsToMove)
            }
        }
    }

    function moveOnePositionUp(index){
        moveOnePosition(index, true);
    }
    function moveOnePositionDown(index){
        moveOnePosition(index, true);
    }
}
