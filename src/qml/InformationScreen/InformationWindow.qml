import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12


ApplicationWindow {
    id:window
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    onVisibilityChanged: console.log("visibility is now",visibility)
    onVisibleChanged: console.log("visible",visible)
    header: Rectangle{
        color: "red"
        height: 30
        visible: true
    }
    SpotifyTrack{
        id: currentSpotifyTrack
        visible: true//spotify.currentUser !== null
        coverImageSource: spotify.currentTrack ? spotify.currentTrack.album.images.data(spotify.currentTrack.album.images.index(0,0)).url : ""
        text: spotify.currentTrack ? spotify.currentTrack.name + " ("+spotify.currentTrack.artists.data(spotify.currentTrack.artists.index(0,0)).name+")<br><font size=\"12\"> "+spotify.currentTrack.album.albumType + ": " + spotify.currentTrack.album.name + "("+spotify.currentTrack.album.releaseDate+")"+" </font>": "No Track is playing"
        height: 50
        width: parent.width
        Timer{
            interval: 1000
            repeat: true
            running: true
            onTriggered: {
                if(spotify.currentPlayingStatus && spotify.currentTrack){
                    currentSpotifyTrack.progress = spotify.currentPlayingStatus.progressMs/spotify.currentTrack.durationMs;

                }
            }
        }
    }
    /*Lauftext{
            text: "TEST hihihi"
            speed: 1
            width: window.width
            height: 30
            y: 60
        }*/

    /*footer: SpotifyTrack{
            id: currentSpotifyTrack
            visible: spotify.currentUser !== null
            coverImageSource: spotify.currentTrack ? spotify.currentTrack.album.images[0] : ""
            text: spotify.currentTrack ? spotify.currentTrack.name + " ("+spotify.currentTrack.artists[0]+")<br><font size=\"12\"> "+spotify.currentTrack.album.albumType + ": " + spotify.currentTrack.album.name + "("+spotify.currentTrack.album.releaseDate+")"+" </font>": ""
            height: 50
            Timer{
                interval: 1000
                repeat: true
                running: true
                onTriggered: {
                    if(spotify.currentPlayingStatus && spotify.currentTrack){
                        currentSpotifyTrack.progress = spotify.currentPlayingStatus.progressMs/spotify.currentTrack.durationMs;
                    }
                }
            }
        }*/

}
