<?php 
    if(isset($_POST['boton'])){
        $fileName = $_FILES['file']['name'];
        $tmpFileName = $_FILES['file']['tmp_name'];
        $ext = substr($fileName,strrpos($fileName, "."));
        if($ext == ".obj"){
            if(move_uploaded_file($tmpFileName, "../../C++/file_obj_LC3/$fileName")){
                // print('<h1>File $fileName moved successful</h1>');
                echo "File".$fileName."moved successful";
            }else{
                // print('<h1>File $fileName could not be moved</h1>');
                print "File".$fileName."could not be moved";
            }
        }else{
            print "Invalid file extension";
        }
    }else{
        print "Data no found";
    }
 ?>