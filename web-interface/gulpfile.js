var gulp 	= require('gulp'),
    gutil 	= require('gulp-util'),
    minifycss 	= require('gulp-minify-css'),
    uglify 	= require('gulp-uglify'),
    rename 	= require('gulp-rename'),
    clean 	= require('gulp-clean'),
    concat 	= require('gulp-concat'),
    notify 	= require('gulp-notify'),
    ejs		= require('gulp-ejs'),
    livereload 	= require('gulp-livereload'),
    htmlmin 	= require('gulp-htmlmin'),
    markdown 	= require('gulp-markdown'),
    insert 	= require('gulp-insert');
    

var fs 		= require('fs'),
    pygmentize 	= require('pygmentize-bundled');
		
var scripts = [ "jquery", "underscore", "backbone", "backbone.localstorage",
		"FileSaver","codemirror", "bootstrap","mousetrap",	    	    	    	    
		"api", "standard-libs",	"utils", "documents",
		"*" ]		
var styles = [  "bootstrap", "bootstrap-darkly",
		"codemirror", "codemirror-theme",
		"style" ]		

gulp.task('scripts.minify', function() {
	return gulp.src('source/javascripts/*.js')
		.pipe(rename({ suffix: '.min' }))
		.pipe(uglify())
		.pipe(gulp.dest('source/javascripts/minified/'))
});

gulp.task('scripts.concat', ['scripts.minify'], function() {
	var minlist = scripts.map(function(e){
		if(e != "*") 
			e += ".min.js"
		return e;
	})	
	return gulp.src(minlist, {cwd : "source/javascripts/minified"})
		.pipe(concat('scripts.js'))
		.pipe(gulp.dest('build'))
		.pipe(uglify())
		.pipe(rename({ suffix: '.min' }))
		.pipe(gulp.dest('build'))
});
gulp.task('scripts',['scripts.concat'], function(){
	return notify({ message: 'Scripts task complete' });
});




gulp.task('styles.minify', function() {
	return gulp.src('source/stylesheets/*.css')
		.pipe(rename({ suffix: '.min' }))
		.pipe(minifycss({keepSpecialComments : 0}))
		.pipe(gulp.dest('source/stylesheets/minified/'))
});
gulp.task('styles.concat', ['styles.minify'], function() {
	var minlist = styles.map(function(e){
		if(e != "*") 
			e += ".min.css"
		return e;
	});
	
	return gulp.src(minlist, {cwd : "source/stylesheets/minified"})		
		.pipe(concat('styles.css'))
		.pipe(gulp.dest('build'))
		.pipe(minifycss({keepSpecialComments : 0}))
		.pipe(rename({ suffix: '.min' }))
		.pipe(gulp.dest('build'));
});
gulp.task('styles',['styles.concat'], function(){
	return notify({ message: 'Styles task complete' });
});
var shortcuts = {
	"Documents Management" : {
		"Open script file" : "ctrl+o",
    		"Open text file" : "ctrl+p",
    		
    		"Create script file" : "ctrl+k",
    		"Create text file" : "ctrl+l",    						
    	},
	"Editor" : {    						
    		"Close doc" : "alt+w",
    		"Go to doc <N>" : "alt+<N>",
    		"Right doc": "ctrl+alt+pgup",
    		"Left doc": "ctrl+alt+pgdown",
    		"Save docs": "ctrl+s",
    	},
    	"Interpreter" : {
    		"Run script": "alt+r",
    	},
    	"Navigation" : {
    		"Shortcuts cheat-sheet" : "alt+h"
    	}
}


gulp.task('html.dev',function(){
	return gulp.src("source/templates/index.ejs")
    		.pipe(ejs({
    			title : "Test Suite",
    			config : {
    				styles :  "raw",
    				scripts : "raw",
    				sc : shortcuts
    			},
    			open : '{{',
			close : '}}'
		})).pipe(gulp.dest("source"));

		

});

gulp.task('documentation.html', function(){
	var top = '<html><head><meta charset="UTF-8"><link type="text/css" rel="stylesheet" href="style.css"></head><body><div class="container">'
	var bottom = '</div></body></html>';
	return gulp.src('../documentation/markdown/*.md')
		.pipe(markdown({
			highlight: function (code, lang, callback) {
					pygmentize({ lang: 'javascript', format: 'html'}, code, function (err, result) {
      						callback(err, result.toString());
					})
    			}
  		})) 
		.pipe(insert.wrap(top, bottom))
	        .pipe(gulp.dest('../documentation/html/'));
})
gulp.task('html.build', function() {

	var styles = fs.readFileSync('build/styles.min.css').toString();
	var scripts = fs.readFileSync('build/scripts.min.js').toString();	
	

	
	return gulp.src("source/templates/index.ejs")
    		.pipe(ejs({
    			title : "Test Suite",
    			data : {
    				styles : styles,
    				scripts  : scripts ,
    			},
    			config : {
    				styles : "production_embed",
    				scripts : "production_embed",
    				sc : shortcuts	    			
    			},
    			open : '{{',
			close : '}}'			
		})).pipe(gulp.dest("build"))
		.pipe(htmlmin({collapseWhitespace: true}))
		.pipe(rename({ suffix: '.min' }))
		.pipe(gulp.dest('build'));
});



gulp.task('default',['styles','scripts','html.build']);
