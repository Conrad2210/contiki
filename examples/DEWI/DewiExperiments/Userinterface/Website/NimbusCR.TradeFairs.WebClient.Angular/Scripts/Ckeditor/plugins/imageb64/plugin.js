CKEDITOR.plugins.add('imageb64',
{
    init: function (editor) {
        var pluginName = 'imageb64';
        editor.ui.addButton('imageb64',
            {
                label: 'Image',
                command: 'OpenWindow',
                icon: CKEDITOR.plugins.getPath('imageb64/icons/') + 'imageb64.png'
            });
        var cmd = editor.addCommand('OpenWindow', { exec: showMyDialog });
    }
});

function showMyDialog(editor) {
    //modal html string
    var html = '<div id="myModal" class="modal fade" role="dialog">'
    +'<div class="modal-dialog">'
    +' <div class="modal-content">'
    +'   <div class="modal-header">'
    +'    <button type="button" class="close" data-dismiss="modal">&times;</button>'
    +'     <h4 class="modal-title">Select Image</h4>'
    +'   </div>'
    + '   <div class="modal-body">'
    +'<div id="no_image_selected_err_msg" class="alert alert-danger hide"><strong>Error!</strong> Please select a file to insert.</div>'
    +'<input id="inp" type="file">'
    +'<p id="b64"></p>'
    + '<img id="img" width="100px">'
    +'  </div>'
    + '  <div class="modal-footer">'
    + '  <button type="button" id="cancel_insert" class="btn btn-danger" data-dismiss="modal">Cancel</button> '
    + '<button  type="button" id="insert_image" class="btn-success btn-sm" >Insert</button>'
    +'   </div>'
    +' </div>'
    + ' </div>'
    + '</div>';
    //check if element already exists 
    if ($("#myModal").length == 0) {//doesn't exist
        $(html).appendTo("body");
    } else {
        $('#no_image_selected_err_msg').addClass('hide');
    }
    
    var imageB64 = '';
    var image_selected = false;
    //listen to event change on browse file input
    EL("inp").addEventListener("change", function () {
        if (this.files && this.files[0]) {
            var FR = new FileReader();
            FR.onload = function (e) {
                imageB64 = e.target.result;
                EL("img").src = e.target.result;
                image_selected = true;
                $('#no_image_selected_err_msg').addClass('hide');
              //  EL("b64").innerHTML = e.target.result;
                
            };
            FR.readAsDataURL(this.files[0]);
        }
    }, false);
    //insert button
    $("#insert_image").unbind("click");
    $('#insert_image').on('click', function () {
        if (image_selected) {
            editor.insertHtml('<img src="' + EL("img").src + '">');
            $('#myModal').modal('hide');
        } else {
            $('#no_image_selected_err_msg').removeClass('hide');
           // $('#myModal').modal('show');
        }
       
    });
    //cancel button
    $("#cancel_insert").unbind("click");
    $('#cancel_insert').on('click', function () {
        // $('#myModal').remove();
        $('#myModal').modal('hide');
    });

   // $('#myModal').modal('toggle');
    $('#myModal').modal('show');
    
  
}

function EL(id) { return document.getElementById(id); } // Get el by ID helper function