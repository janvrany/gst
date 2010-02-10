Object subclass: MockA [    
    foo [
    	'foo' printNl.    	
		^#foo  
    ]    
    bar [
    	'bar' printNl.    	
		^#bar  
    ]    
    qux [   
    	'qux' printNl. 	
		^#qux  
    ]
]

MockA subclass: MockB [    
    bar [   
    	'bar->baz' printNl. 	
		^#baz 
    ]    
]

Object subclass: MockC [
	callBarIn: anObject [
		^anObject bar
	]
] 

Object subclass: MockD [
	foo [
		^self baz
	]
	
	bar [
		^self baz
	]
	
	baz [
		'baz' printNl.
		^#baz
	]		
	
	
	bind: methods forMethod: senderMethod [
		| method |		
		methods isEmpty ifTrue:[^nil].
		method := methods first.		
		^(method descriptor selector == #baz)
			ifTrue:[nil]
			ifFalse:[method]			
	]
]

Lookup subclass: MockALookup [

	lookup: selector in: initialSearchClass forMethod: senderMethod [		
		(selector == #bar and:[senderMethod == (MockC >> #callBarIn:)]) ifTrue:
			[^super lookup: #foo in: initialSearchClass forMethod: senderMethod].						
		(selector == #foo) ifTrue:
			[^super lookup: #bar in: initialSearchClass forMethod: senderMethod].
		(selector == #qux) ifTrue:
			[^#()].			
		^super lookup: selector in: initialSearchClass forMethod: senderMethod
	]

]

TestCase subclass: MOPBasicTest [
	| mock |
	
	setUp [
		MockA lookup: nil.
		MockB lookup: nil.
		Behavior flushCache.
		'--' printNl.
	]
	
	test01a [				
		mock := MockA new.
		self 
			assert: mock foo = #foo;
			assert: mock bar = #bar;			  
			assert: mock qux = #qux.
			
		MockA lookup: MockALookup new.
				
		self 
			assert: mock foo = #bar;
			assert: mock bar = #bar.		
		self should:[mock qux] raise: Error.						  	
	]
	
	test01b [		
		mock := MockB new.
				
		self 
			assert: mock foo = #foo;
			assert: mock bar = #baz;			  
			assert: mock qux = #qux.
			
		MockA lookup: MockALookup new.
				
		self 
			assert: mock foo = #baz;
			assert: mock bar = #baz.		
		self should:[mock qux] raise: Error.						  	
	]
		
	test02 [		
		self assert: (MockA lookup == Lookup builtin).
		MockA lookup: MockALookup new.
		self assert: (MockA lookup class == MockALookup).
		self assert: (MockB lookup class == MockALookup).
	]
	
	test03 [
		"Tests proper selector cache behavior"
		mock := MockA new.
		
		self assert: (mock bar) == #bar.
		self assert: (MockC new callBarIn: mock) == #bar.
						
		MockA lookup: MockALookup new.
		
		self assert: (mock bar) == #bar.
		self assert: (MockC new callBarIn: mock) == #foo.		
	]
	
	test04 [
		"Tests binder"

		mock := MockD new.
		self assert: mock foo = #baz.
		self assert: mock bar = #baz.
							
		(MockD >> #foo) descriptor binder: mock.
			
		self should:[mock foo] raise: Error.		
		self assert: mock bar = #baz.
									
	]
]